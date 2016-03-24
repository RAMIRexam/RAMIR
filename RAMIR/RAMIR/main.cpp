/*
Copyright RAMIR (Räkning av människor i rörelse) 
Del1 i Examensarbete - implementering av artikeln "Automatic counting of interacting people by using a single uncalibrated camera".

Emil Andersson and Niklas Schedin 
*/


#include <stdlib.h>
#include <iostream>
#include <Windows.h>
#include "Settings.hpp"
#include "Tracker.hpp"
#include "MyWindows.hpp"

#include <opencv2\core.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\background_segm.hpp>

using namespace cv;
using namespace std;





Ptr<BackgroundSubtractor> pMOG;
Mat filterResult;													//Final imageprocessing image, text is painted in this image

int verticalEelinePos;											//vertical entry/exit-line position in the image. Value can be changed below.


vector<vector<Point>> myFindContours(Mat src);
vector<Rect> getAllRects(vector<vector<Point>> contours);
vector<Blob> createBlobs(Mat image, vector<vector<Point>> contours);
vector<Tracker*> intersectionTest(vector<Blob>* blobs, vector<Tracker*> trackers);
vector<Tracker*> tracking(vector<Blob> blobs);
vector<Tracker*> trackerSurvivalTest(vector<Tracker*> trackers);
string type2str(int type);
Mat elipseFilter(Mat bgsub);
Mat resizeImage(Mat frame, double divider);


double startClocking();
void stopClocking(double t);

void countTrackers();
void paintTrackerinfo(Mat paintImage);

vector<Tracker*> trackers;										//Contains trackers which hasn't been counted yet.
vector<Tracker*> ACTrackers;									//Already counted trackers. When a tracker is counted it shall be moved from trackers to this vector

double _imageDivider = 3;
Scene* scene;
Rect trackingRect;

int rightMovCnt;												//Person moved from left to right detected this many times
int leftMovCnt;													//Person moved from right to left detected this many times
int trackerLife;												//How many images its accaptable for a tracker to not find a belonging blob
int minTrackToBeCounted;										//How many tracks an object has to be tracked before it accepted into the ACTracker

double runTime;													//Variables for counting FPS and cycletime
int FPS;														//
int lastFPS;													//
vector<int> FPShistory;											//
int FPSmean;													//

bool paintRect = true;

bool erodeFilter_BOOL;											//Defines which filter shall be used
bool elipseFilter_BOOL;											//


int main()
{	
	
	//Print which filters that are available
	namedWindow("windows");
	Mat infoImage = Mat::zeros(200, 600, CV_8UC1);
	putText(infoImage, "Please chose filter: " , Point(20, 40), FONT_HERSHEY_DUPLEX, 1, Scalar(255, 255, 255), 1);
	putText(infoImage, "E - Erode/Dilate filter: " , Point(20, 80), FONT_HERSHEY_DUPLEX, 1, Scalar(255, 255, 255), 1);
	putText(infoImage, "R - Rectangle filter: " , Point(20, 120), FONT_HERSHEY_DUPLEX, 1, Scalar(255, 255, 255), 1);
	moveWindow("windows", 0, 0);
	imshow("windows", infoImage);
	infoImage.release();


	//Choose filter
	switch (waitKey(0))
	{
		//E-button, Erode/Dilate filter button
	case 101:
		erodeFilter_BOOL = true;										//Set Erode/Dilate filter
		elipseFilter_BOOL = false;										//
		break;

		//R-button, Elipse filter button
	case 114:
		erodeFilter_BOOL = false;										//
		elipseFilter_BOOL = true;										//Set Elipse filter
		break;
	}




	Mat frame, prevBG, bgsub, erode;
	Rect lastRect;

	Settings::loadSettings();

	MyWindows windows(1500);									//Used to handle image-representation. Argument = screenWidth

	rightMovCnt = 0;
	leftMovCnt = 0;
	trackerLife = 4;
	minTrackToBeCounted = 0;

	runTime = 0;
	FPS = 0;
	lastFPS = 0;

	String video = "C:\\Users\\Emil\\Videos\\humanwalk.mp4";
	VideoCapture cap(video);

	if (!cap.isOpened())
		return -1;


	pMOG = createBackgroundSubtractorMOG2(Settings::getA(), (((double)Settings::getB()) / 10), false);


	//--------------TESTING--------------------------------
	int hbins = 30, sbins = 32;
	int histSize[] = { hbins, sbins };
	float hranges[] = { 0, 180 };
	float sranges[] = { 0, 256 };
	const float* ranges[] = { hranges, sranges };
	int channels[] = { 0, 1 };

	cap >> frame; //TESTING!
	pMOG->apply(frame, bgsub); //TESTING!
	//------------------------------------------------------

	Settings::init(&pMOG);


	/*************************************************************************************************************************************/
	//	Run ones outside the loop to set the image layout
	//

	cap >> frame;
	Mat out = resizeImage(frame, _imageDivider);

	int widthDivider = 3;															//sets the ROI's width (ROI defined by the user)
	int heightDivider = 10;															//sets the ROI's height (ROI defined by the user)

	int trackingROI_x = out.cols / widthDivider;									//Define the coordinates for the ROI defined by the user
	int trackingROI_y = out.rows / heightDivider;
	
	int	notTrackedWidth = (out.cols / widthDivider) * 2;							//Used to merge the filterResult-Mat and the raw image
	int trackingROI_width = out.cols - notTrackedWidth;
	int trackingROI_height = out.rows - ((out.rows / heightDivider) * 2);

	verticalEelinePos = out.cols / 2;												//User option, where shall the eeline be placed?
	

	Rect ROI_Rect = Rect(trackingROI_x, trackingROI_y, trackingROI_width, trackingROI_height);
	Mat ROI_DefbyUser = out(ROI_Rect);
	
	
	scene = new Scene(verticalEelinePos, trackingROI_y, verticalEelinePos, trackingROI_y + trackingROI_height, ROI_DefbyUser, ROI_Rect);
	
	//OLD
	//scene = Scene(out.cols / 2, 0, out.cols / 2, out.rows, out);					//frame is not used as ROI yet (whithinROI-check not implemented)
	/**************************************************************************************************************************************/

	

	while (true)
	{	
		double cycleTime = startClocking();														//Starts the clock in order to calculate FPS

		cap >> frame;

		if (frame.empty()){																		//Used when the end of the video is reached
			cap.release();
			cap.open(video);
			
			pMOG = createBackgroundSubtractorMOG2(Settings::getA(), (((double)Settings::getB()) / 10), false);
			
			cap >> frame;
		}

		
		Mat out = resizeImage(frame, _imageDivider);													//Resizes the raw image.
		//colorImage = Mat::zeros(out.rows, out.cols, CV_8UC3);

		trackingRect = Rect(trackingROI_x, trackingROI_y, trackingROI_width, trackingROI_height);		//create a rect with the coordinates of the ROIs defined by the user
		//Mat trackingROI = out.clone();
		//trackingROI(trackingRect);												//The ROI defined by the user. Outside of this ROI the blobs isn't tracked.

		Mat trackingROI = out(trackingRect);



		



		prevBG = bgsub;
		pMOG->apply(trackingROI, bgsub);											//Set which image that shall be processed.


		//double val = compareHist(hist1, hist2, CV_COMP_BHATTACHARYYA);
		//double val2 = compareHist(hist1, hist2, CV_COMP_INTERSECT);
		//cout << "Bhattacharyya: " << val << "\tHellinger" << val2 << endl;
		//windows.feedImages("BGS", bgsub);


		assert((erodeFilter_BOOL && !elipseFilter_BOOL) || (!erodeFilter_BOOL && elipseFilter_BOOL));		//Asserts that one filter is chosen and only one

		if (erodeFilter_BOOL) {
			cv::erode(bgsub, erode, Settings::getErodeElement());
			cv::dilate(erode, filterResult, Settings::getDilateElement());			//filterResult is the dilatad image
		}
		if (elipseFilter_BOOL) {
			filterResult = elipseFilter(bgsub);
		}

		
		//cvtColor(filterResult, colorImage, CV_GRAY2BGR, 3);									//convert grayscale image to colorimage
		vector<vector<Point>> contours = myFindContours(filterResult);						//function uses findContours() to get all contours in the image
		vector<Blob> blobs = createBlobs(trackingROI, contours);				//create blobs (ROI, RECT, HIST) from all rects

		trackers = tracking(blobs);										//tracks the blobs
		countTrackers();												//moves tracker objects from trackers to ACTrackers if they has passed the eeline
		
		

		


		Mat filterResult2;
		cvtColor(filterResult, filterResult2, CV_GRAY2BGR, 3);



		Mat dd = out(trackingRect);
		dd = dd | filterResult2;


		paintTrackerinfo(out);												//prints info about all detected trackers in the image

		/*
		Mat showImage;
		int extraBlackWidth = notTrackedWidth / 2;						//notTrackedWidth is the spaces on the left and on the right of the tracked area added together
		Mat extraBlack = Mat::zeros(trackingROI_height, extraBlackWidth, CV_8UC1);
		vector<Mat> mergeMats;
		mergeMats.push_back(extraBlack);
		mergeMats.push_back(filterResult);
		mergeMats.push_back(extraBlack);
		
		hconcat(mergeMats, showImage);

		//extraBlack = Mat::zeros(trackingROI_height, extraBlackWidth, CV_8UC1);

		Mat test = showImage(Rect(100, 100, 100, 100));
		//showImage = showImage | out;
		*/



		moveWindow("Windows", 0, 0);
		imshow("Windows", out);									//Show the result image.








		//View images in a special class
		//
		//windows.feedImages("Windows", frame);
		//windows.feedImages("Dilate Window", dilate);
		//windows.feedImages("Contours", colorImage);
		//windows.showImages();

		switch (waitKey(1))
		{
			//Esc-button
			case 27:
				cap.release();
				destroyAllWindows();
			
				Settings::saveSettings();

				return 0;
				break;
		}

		stopClocking(cycleTime);													//Stops the cycle timer. (A part in the process of calculating FPS)
	}
	return 0;
}

/**************************************************************************************************************************************
/	Resizes an image. Greater value on argument "divider" gives a smaller image
**************************************************************************************************************************************/
Mat resizeImage(Mat frame, double divider) {
	Mat out;																//Resize the image													//
																			//
	int width = frame.cols / divider;										//
	int height = frame.rows / divider;										//
																			//
	Size size(width, height);												//
	resize(frame, out, size);												//
	return out;
}

Mat elipseFilter(Mat bgsub) {
	Mat temp1;
	Mat temp2;
	Mat temp3;

	filterResult = Mat::zeros(bgsub.size(), CV_8UC1);
	temp1 = bgsub.clone();


	const int kernelSize = 9;												//Width and height on kernel which will be convolved with the backgroundsubtracted image
	const int kernelArea = kernelSize * kernelSize;
	const double accPerc = 0.5;												//(accPerc*100 = Percentage) If the convolution divided by the kernelArea is greater than this value, something will be painted on the resultImage.
	const double limIntensVal = kernelArea * accPerc;


	threshold(temp1, temp2, 100, 1, THRESH_BINARY);							//temp_bgsub will contain 0 or 255 values

	Mat kernel = Mat::ones(kernelSize, kernelSize, CV_8UC1);				//Construct kernel that will be convolved with the image
	filter2D(temp2, temp3, -1, kernel);										//Conolution on BGS-image and kernel


	const int paintRect_size = 4;


	for (int i = paintRect_size; i < temp3.cols - paintRect_size; i++) {
		for (int k = paintRect_size; k < temp3.rows - paintRect_size; k++) {
			Scalar test = temp3.at<uchar>(k, i);

			if (test[0] > limIntensVal) {
				rectangle(filterResult, Point(i, k), Point(i + paintRect_size, k + paintRect_size), Scalar(255, 255, 255), CV_FILLED, 1, 0);
			}
		}
	}

	return filterResult;
}


/**************************************************************************************************************************************
/	Paints how many times the trackers have found their blobs in the upper left corner
/	Paints right and left counters in upper middle of the image
**************************************************************************************************************************************/
void paintTrackerinfo(Mat paintImage) {
	
	int dText = 20;
	int trackername = 1;

	int minDuration = 10;
	int paintRectDuration = 15;


	/*
	for (Tracker *t : trackers) {

		//Paints how many detections a tracker has made if it exceds or is equal to minDuration. Painted in upper left corner
		if (t->getDuration() >= minDuration) {
			putText(paintImage, "Tracker " + to_string(trackername) + ": Number detections " + to_string(t->getDuration()), Point(20, 20 + dText), FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 255, 0), 1);
			dText += 20;
			trackername++;

			if (t->getDuration() > paintRectDuration) {
				rectangle(paintImage, t->getLastBlob().getRect().tl(), t->getLastBlob().getRect().br(), Scalar(0, 0, 255), 2, 8, 0);
			}
		}
	}
	*/


	//Paint centroid for the trackers
	for (Tracker *t : trackers) {
		int startx = scene->getStartPos().x;
		int starty = scene->getStartPos().y;
		int ROIx = t->getLastBlob().getCent().x;
		int ROIy = t->getLastBlob().getCent().y;

		circle(paintImage, Point2f(startx + ROIx, starty + ROIy), 10, Scalar(0, 0, 255), 2, 8, 0);
	}

	//Paint centroid for the ACTrackers
	for (Tracker *t : ACTrackers) {
		int startx = scene->getStartPos().x;
		int starty = scene->getStartPos().y;
		int ROIx = t->getLastBlob().getCent().x;
		int ROIy = t->getLastBlob().getCent().y;

		circle(paintImage, Point2f(startx + ROIx, starty + ROIy), 10, Scalar(0, 255, 0), 2, 8, 0);
	}




	//Paint ROI defined by user
	rectangle(paintImage, trackingRect.tl(), trackingRect.br(), Scalar(255, 255, 255), 2, 8, 0);

	//Paint entry/exit-line. the ROI defined by the user sets the upper and lower limits.
	line(paintImage, Point(verticalEelinePos, trackingRect.y), Point(verticalEelinePos, (trackingRect.y + trackingRect.height)), Scalar(255, 0, 0), 2);




	//paint black background to write info on
	Rect r(0, 0, 200, 100);
	Mat black = Mat::zeros(r.height, r.width, CV_8UC3);
	Mat paintROI = paintImage(r);
	paintROI = paintROI & black;


	//Paints right and left counters in upper middle of the image
	putText(paintImage, "Right Counter: " + to_string(rightMovCnt), Point(20, 20), FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 255, 255), 1);
	putText(paintImage, "Left Counter: " + to_string(leftMovCnt), Point(20, 40), FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 255, 255), 1);

	//Paints frames per second, FPS
	putText(paintImage, "FPS: " + to_string(lastFPS), Point(20, 60), FONT_HERSHEY_DUPLEX, 0.5, Scalar(100, 100, 255), 1);
	putText(paintImage, "Mean FPS: " + to_string(FPSmean), Point(20, 80), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255, 100, 100), 1);


	//Print how many ACTrackers there is
	if (ACTrackers.size() == 0) {
		putText(paintImage, "num ACTrackers: 0", Point(800, 20), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255, 0, 255), 1);
	}
	else {
		for (Tracker* t : ACTrackers) {
			putText(paintImage, "num ACTrackers: " + to_string(ACTrackers.size()), Point(800, 20), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255, 0, 255), 1);
		}
	}
}

/**************************************************************************************************************************************
/	Checks whether a person shall be counted (moved from trackers to ACTrackers)
/	Counts the person if it has moved from the sSOL (start side of line) to the other side. Possible sides: LEFTSIDE_OFLINE / RIGHTSIDE_OFLINE
**************************************************************************************************************************************/
void countTrackers() {
/*
/	Tests:
/		(1) tracker.size in == tracker.size out
/		(2) number of trackers in (trackers + ACtrackers) == number trackers out (trackers + ACtrackers)
/		(3)	trackers start side of line and current side of line must be either LEFTSIDE_OFLINE or RIGHTSIDE_OFLINE
*/
	
	int nTrackers_DEBUG = trackers.size() + ACTrackers.size();							//(2) DEBUG


	for (Tracker* t : trackers) {														//Update the trackers current side of line
		t->curSOL = scene->LSCheck(t->getLastBlob());
	}


	if (trackers.size() > 0) {															//(1) DEBUG, used to check so trackers is'nt 0 at end
		vector<Tracker*> tempTrackers;

		while (trackers.size() > 0) {
			Tracker* t = trackers.back();
			trackers.pop_back();

			assert(t->curSOL == RIGHTSIDE_OFLINE || t->curSOL == LEFTSIDE_OFLINE);		//(3) DEBUG
			assert(t->staSOL == RIGHTSIDE_OFLINE || t->staSOL == LEFTSIDE_OFLINE);		//(3) DEBUG

			if (t->curSOL != t->staSOL && t->getDuration() > minTrackToBeCounted) {		//tracker shall be counted (has moved from one side to another)
				if (t->curSOL == RIGHTSIDE_OFLINE) { rightMovCnt++; }					//increment movement from left to right counter
				else{leftMovCnt++;}														//increment movement from right to left counter

				ACTrackers.push_back(t);
			}
			else {
				tempTrackers.push_back(t);
			}
		}

		trackers = tempTrackers;
	}

	assert(nTrackers_DEBUG == trackers.size() + ACTrackers.size());						//(2) DEBUG

}


/**************************************************************************************************************************************
/	Trackers will be filled with blobs, one blob for each image.
/	if a tracker cannot find a blob that matches the tracker, it will be filled with a "emptyblob"
/	ACTrackers contains already counted trackers. First the blobvector will be checkt for blobs belonging to trackers in ACTrackers
/	When blobs belonging to ACTrackers are removed, the rest of the blobs will be new trackers or moved to trackers (not yet counted trackers)
**************************************************************************************************************************************/
vector<Tracker*> tracking(vector<Blob> blobs) {
/*
/	Tests:
/		(1) Check if all trackers is processed at the end
/
*/


	if (ACTrackers.size() > 0) {									//if there exists already counted trackers, the blobs belonging to this trackers must first be removed
		ACTrackers = intersectionTest(&blobs, ACTrackers);			//intersectionTest will move blobs to "already counted trackers"
	}


	if (trackers.size() == 0) {
		for (Blob b : blobs) {										//No trackers exists. All blobs will turn to a tracker
			int lineSide = scene->LSCheck(b);
			Tracker *t = new Tracker(lineSide, b, trackerLife);

			t->processed = true;
			trackers.push_back(t);

		}
	}
	
	else {
		trackers = intersectionTest(&blobs, trackers);				//there already exists trackers, intersectionTest will move blobs to trackers

		for (Blob b : blobs) {										//iterate throught the rest of the blobs and create trackers for them
			int lineSide = scene->LSCheck(b);
			Tracker *t = new Tracker(lineSide, b, trackerLife);

			t->processed = true;
			trackers.push_back(t);

		}

		for (Tracker *t : trackers) {
			if (!t->processed) {									//if tracker is not processed, fill it with emptyblob
				t->fillWithEmptyBlob();
				t->processed = true;
			}
		}
	}


	for (Tracker *t : trackers) {
		assert(t->processed == true);								//(1) DEBUG
	}

	trackers = trackerSurvivalTest(trackers);						//decrement trackerlife. If trackerlife is 0 the tracker is removed
	ACTrackers = trackerSurvivalTest(ACTrackers);					//decrement trackerlife. If trackerlife is 0 the tracker is removed


	for (Tracker *t : trackers) {t->processed = false;}				//reset processed for next iteration
	for (Tracker *t : ACTrackers) {t->processed = false;}			//reset processed for next iteration


	return trackers;
}

/**************************************************************************************************************************************
/	Checks how many lifes a tracker has left. If zero the tracker will be destroyed. A trackers lifes depends on its emptyblobs.
**************************************************************************************************************************************/
vector<Tracker*> trackerSurvivalTest(vector<Tracker*> trackers) {
	//DEBUGGING
	int numbertrackers = trackers.size();
	int counter = 0;

	//check if trackers shall survive
	vector<Tracker*> tempTrackers;

	while (trackers.size() > 0) {
		counter++;

		Tracker *t = trackers.back();

		trackers.pop_back();

		if (t->survivalTest()) {
			tempTrackers.push_back(t);
		}
		else {
			delete t;
		}


	}

	trackers = tempTrackers;

	assert(trackers.size() == tempTrackers.size());
	assert(counter == numbertrackers);

	return trackers;
}

/**************************************************************************************************************************************
/	Iterates throught all trackers and checks if their last matched blob intersect with one of them in the scene. If a blob intersects
/	it will be added to the tracker and removed from the blobvector.
**************************************************************************************************************************************/
vector<Tracker*> intersectionTest(vector<Blob>* blobs, vector<Tracker*> trackers) {
	/*
	/	Tests:
	/		(1) Blob from getLastBlob, shall never return an emptyblob
	/		(2)
	/
	*/

	int i = trackers.size() - 1;
	while(i >= 0){																//Check tracker from front (oldest tracker shall be checked first)
		Tracker* t = trackers[i];
		
		vector<Blob>* restBlobs = new(vector<Blob>);
		Blob bestBlob;															// constructs an emptyblob (WORKS)

		double minBhatta = DBL_MAX;
		vector<Mat> isBlobs;													//InterSecting Blobs

		int blobcounter = 0;													//DEBUG
		int numberBlobs = blobs->size();											//DEBUG

		while (blobs->size() > 0) {
			blobcounter++;														//DEBUG

			Blob b = blobs->back();
			blobs->pop_back();													//Pop makes shure other intersecting blobs than the best is ignored

			assert(!t->getLastBlob().emptyBlob);								//(1) DEBUG

			if ((b.getRect() & t->getLastBlob().getRect()).area() > 0) {		//intersection test
				Mat hist1 = b.getHist();
				Mat hist2 = t->getLastBlob().getHist();


				double bhatta = compareHist(b.getHist(), t->getLastBlob().getHist(), CV_COMP_BHATTACHARYYA);

				if (bhatta < minBhatta) {
					minBhatta = bhatta;

					bestBlob = b;
				}
			}
			else { restBlobs->push_back(b); }
		}
		assert(blobcounter == numberBlobs);					//if blob is pop'ed, will all blobs still be iterated?

		int testsize = restBlobs->size();					//Debugvariable

		blobs = restBlobs;									//memcpy(&blobs, &restBlobs, sizeof(restBlobs));

		//assert(blobs != restBlobs);							//DEBUG
		assert(blobs->size() == testsize);					//DEBUG

															//If intersection
		if (!bestBlob.emptyBlob) { t->fillWithBlob(bestBlob); }
		else { t->fillWithEmptyBlob(); }

		t->processed = true;
		i--;
	}
	return trackers;
}


/**************************************************************************************************************************************
/INFO
/
/   Calculates a bounding rectangle for a blob from its contour
/	Calculates a ROI at the rectangles spot
/	Calculates a histogram at the ROI
/	Calculates the centroid for a blob from its contour
/	
/
/ARGUMENTS
/
/	Mat image =							original color image
/	vector<vector<Point>> contours =	points that define blobs location
/	
**************************************************************************************************************************************/
vector<Blob> createBlobs(Mat image, vector<vector<Point>> contours) {
	/*
	/	Tests:
	/		(1) the rectangles height and width must be greater than 0
	/
	*/


	//These parameters are taken from an example code
	//------------------------------------------------------
	int hbins = 30, sbins = 32;
	int histSize[] = { hbins, sbins };
	float hranges[] = { 0, 180 };
	float sranges[] = { 0, 256 };
	const float* ranges[] = { hranges, sranges };
	int channels[] = { 0, 1 };
	Mat hist;
	vector<Blob> blobs;
	//------------------------------------------------------



	//Calculate blob data
	for (vector<Point> cont : contours) {
		


		//calculates a bounding rectangle around the contour
		Rect rect = boundingRect(cont);
		assert(rect.width > 0 && rect.height > 0);							//(1) DEBUG 


		//Creates a ROI on same spot as the rectangle
		Mat blobROI = image.clone();											
		blobROI(rect);


		//Calculates the histogram for the ROI
		calcHist(&blobROI, 1, channels, Mat(), hist, 2, histSize, ranges);


		//Find the centroid at the contour
		Moments m = moments(cont, false);									
		Point2f cent = Point2f(m.m10 / m.m00, m.m01 / m.m00);




		//Finally create the blob
		Blob b(hist, rect, blobROI, cent);
		blobs.push_back(b);

	}

	return blobs;
}

/**************************************************************************************************************************************
/	Returns a vector containing all rectangles around each object.
**************************************************************************************************************************************/
vector<Rect> getAllRects(vector<vector<Point>> contours) {

	vector<Rect> rects;
	for (vector<Point> cont : contours) {
		//calculate the rectangle around the contour
		Rect r = boundingRect(cont);
		rects.push_back(r);
		
		//paint the rectangle:
		if(paintRect == true)
			rectangle(filterResult, r.tl(), r.br(), Scalar(0, 0, 255), 2, 8, 0);
	}
	
	return rects;
}

/**************************************************************************************************************************************
/	Used to reduce code in main
**************************************************************************************************************************************/
vector<vector<Point>> myFindContours(Mat src) {
	Mat src_cpy;
	src.copyTo(src_cpy);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours(src_cpy, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	//Check if the area on the contour is big enough
	vector<vector<Point>> retContours;
	for (unsigned int i = 0; i < contours.size(); i++){
		int area = moments(contours[i], false).m00;
		if (area >= Settings::getE())
			retContours.push_back(contours[i]);
	}

	return retContours;
}

//Function for determining which type an matrix is.
string type2str(int type) {
	string r;

	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch (depth) {
	case CV_8U:  r = "8U"; break;
	case CV_8S:  r = "8S"; break;
	case CV_16U: r = "16U"; break;
	case CV_16S: r = "16S"; break;
	case CV_32S: r = "32S"; break;
	case CV_32F: r = "32F"; break;
	case CV_64F: r = "64F"; break;
	default:     r = "User"; break;
	}

	r += "C";
	r += (chans + '0');

	return r;
}

/**************************************************************************************************************************************
/	Starts the clock in the beginning of the program in order to calculate FPS. stopClocking() shall be called when the cycle is done
**************************************************************************************************************************************/
double startClocking() {
	if (runTime > 1) {												//If one second has past
		lastFPS = FPS;
		FPShistory.push_back(FPS);

		for (int fps : FPShistory)
			FPSmean += fps;

		FPSmean /= FPShistory.size();


		FPS = 0;
		runTime = 0;
	}
	else { FPS++; }
	return (double)getTickCount();								//Start timer clock. Used to calculate FPS
}
/**************************************************************************************************************************************
/	Stops the clock in the end of the program in order to calculate FPS. startClocking() shall be called when the cycle starts
**************************************************************************************************************************************/
void stopClocking(double t) {
	t = ((double)getTickCount() - t) / getTickFrequency();			//how long time a cycle takes in seconds
	runTime += t;													//how long time the program has run this cycle
}