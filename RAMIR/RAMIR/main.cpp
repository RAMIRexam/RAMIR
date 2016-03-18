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
Mat colorImage;													//Final imageprocessing image, text is painted in this image

int verticalEelinePos;											//vertical entry/exit-line position in the image. Value can be changed below.


vector<vector<Point>> myFindContours(Mat src);
vector<Rect> getAllRects(vector<vector<Point>> contours);
vector<Blob> createBlobs(Mat image, vector<vector<Point>> contours);
vector<Tracker*> intersectionTest(vector<Blob> blobs, vector<Tracker*> trackers);
vector<Tracker*> tracking(vector<Blob> blobs);
vector<Tracker*> trackerSurvivalTest(vector<Tracker*> trackers);
void countTrackers();
void paintTrackerinfo();

vector<Tracker*> trackers;										//Contains trackers which hasn't been counted yet.
vector<Tracker*> ACTrackers;									//Already counted trackers. When a tracker is counted it shall be moved from trackers to this vector

Scene scene;

int rightMovCnt;												//Person moved from left to right detected this many times
int leftMovCnt;													//Person moved from right to left detected this many times
int trackerLife;												//How many images its accaptable for a tracker to not find a belonging blob
int minTrackToBeCounted;										//How many tracks an object has to be tracked before it accepted into the ACTracker

bool paintRect = true;


int main()
{
	Mat frame, prevBG, bgsub, erode, dilate, gray, temp, hist1, hist2, lastHist, tempHist, lastROI;
	Rect lastRect;

	Settings::loadSettings();

	MyWindows windows(1500);									//Used to handle image-representation. Argument = screenWidth

	rightMovCnt = 0;
	leftMovCnt = 0;
	trackerLife = 4;
	minTrackToBeCounted = 0;

	String video = "videoplayback2.mp4";
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

	scene = Scene(frame.cols/2 , 0, frame.cols / 2, frame.rows, frame);			//frame is not used as ROI yet (whithinROI-check not implemented)

	Settings::init(&pMOG);

	while (true)
	{	
		
		cap >> frame;
		verticalEelinePos = frame.cols / 2;										//User option, where shall the eeline be placed?

		if (frame.empty())
		{
			cap.release();
			cap.open(video);
			
			pMOG = createBackgroundSubtractorMOG2(Settings::getA(), (((double)Settings::getB()) / 10), false);
			
			cap >> frame;
		}
		prevBG = bgsub;
		pMOG->apply(frame, bgsub);


		//double val = compareHist(hist1, hist2, CV_COMP_BHATTACHARYYA);
		//double val2 = compareHist(hist1, hist2, CV_COMP_INTERSECT);
		//cout << "Bhattacharyya: " << val << "\tHellinger" << val2 << endl;

		windows.feedImages("BGS", bgsub);

		cv::erode(bgsub, erode, Settings::getErodeElement());
		cv::dilate(erode, dilate, Settings::getDilateElement());

		cvtColor(dilate, colorImage, CV_GRAY2BGR, 3);

		vector<vector<Point>> contours = myFindContours(dilate);		//function uses findContours() to get all contours in the image
		vector<Blob> blobs = createBlobs(frame, contours);				//create blobs (ROI, RECT, HIST) from all rects

		trackers = tracking(blobs);										//tracks the blobs
		countTrackers();												//moves tracker objects from trackers to ACTrackers if they has passed the eeline
		paintTrackerinfo();												//prints info about all detected trackers in the image
		

		

																		//Paint entry/exit-line
		line(colorImage, Point(verticalEelinePos, 0), Point(verticalEelinePos, colorImage.rows), Scalar(255, 0, 0), 2);

		//Paint centroid
		//ellipse(colorImage, p, Size(40, 40), 0, 0, 360, Scalar(0, 255, 0), 1);


		

		//namedWindow("Window", WINDOW_AUTOSIZE);
		//namedWindow("BGS", WINDOW_AUTOSIZE);
		namedWindow("Contours", CV_WINDOW_AUTOSIZE);
		
		Mat out;
		double divider = 2.2;

		int width = colorImage.cols / divider;
		int height = colorImage.rows / divider;

		Size size(width, height);

		resize(colorImage, out, size);

		moveWindow("Windows", 0, 0);
		imshow("Windows", out);



		

		//View images
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
	}
	return 0;
}

/**************************************************************************************************************************************
/	Paints how many times the trackers have found their blobs in the upper left corner
/	Paints right and left counters in upper middle of the image
**************************************************************************************************************************************/
void paintTrackerinfo() {
	
	int dText = 20;
	int trackername = 1;

	int minDuration = 10;
	int paintRectDuration = 15;


	for (Tracker *t : trackers) {

		//Paints how many detections a tracker has made if it exceds or is equal to minDuration. Painted in upper left corner
		if (t->getDuration() >= minDuration) {
			putText(colorImage, "Tracker " + to_string(trackername) + ": Number detections " + to_string(t->getDuration()), Point(20, 20 + dText), FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 255, 0), 1);
			dText += 20;
			trackername++;

			if (t->getDuration() > paintRectDuration) {
				rectangle(colorImage, t->getLastBlob().getRect().tl(), t->getLastBlob().getRect().br(), Scalar(0, 0, 255), 2, 8, 0);
			}
		}
	}


	//Paints right and left counters in upper middle of the image
	putText(colorImage, "Right Counter: " + to_string(rightMovCnt), Point(500, 20), FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 255, 255), 1);
	putText(colorImage, "Left Counter: " + to_string(leftMovCnt), Point(500, 40), FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 255, 255), 1);


	//Print how many ACTrackers there is
	if (ACTrackers.size() == 0) {
		putText(colorImage, "num ACTrackers: 0", Point(800, 20), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255, 0, 255), 1);
	}
	else {
		for (Tracker* t : ACTrackers) {
			putText(colorImage, "num ACTrackers: " + to_string(ACTrackers.size()), Point(800, 20), FONT_HERSHEY_DUPLEX, 0.5, Scalar(255, 0, 255), 1);
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
		t->curSOL = scene.LSCheck(t->getLastBlob());
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
		ACTrackers = intersectionTest(blobs, ACTrackers);			//intersectionTest will move blobs to "already counted trackers"
	}


	if (trackers.size() == 0) {
		for (Blob b : blobs) {										//No trackers exists. All blobs will turn to a tracker
			Tracker *t = new Tracker(b, trackerLife, &scene);
			t->processed = true;
			trackers.push_back(t);

		}
	}
	
	else {
		trackers = intersectionTest(blobs, trackers);				//there already exists trackers, intersectionTest will move blobs to trackers

		for (Blob b : blobs) {										//iterate throught the rest of the blobs and create trackers for them
			Tracker *t = new Tracker(b, trackerLife, &scene);
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
vector<Tracker*> intersectionTest(vector<Blob> blobs, vector<Tracker*> trackers) {
	/*
	/	Tests:
	/		(1) Blob from getLastBlob, shall never return an emptyblob
	/		(2)
	/
	*/

	int i = trackers.size() - 1;
	while(i >= 0){																//Check tracker from front (oldest tracker shall be checked first)
		Tracker* t = trackers[i];
		
		vector<Blob> restBlobs;
		Blob bestBlob;															// constructs an emptyblob (WORKS)

		double minBhatta = DBL_MAX;
		vector<Mat> isBlobs;													//InterSecting Blobs

		int blobcounter = 0;													//DEBUG
		int numberBlobs = blobs.size();											//DEBUG

		while (blobs.size() > 0) {
			blobcounter++;														//DEBUG

			Blob b = blobs.back();
			blobs.pop_back();													//Pop makes shure other intersecting blobs than the best is ignored

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
			else { restBlobs.push_back(b); }
		}
		assert(blobcounter == numberBlobs);					//if blob is pop'ed, will all blobs still be iterated?

		int testsize = restBlobs.size();					//Debugvariable

		blobs = restBlobs;									//memcpy(&blobs, &restBlobs, sizeof(restBlobs));

		assert(&blobs != &restBlobs);						//DEBUG
		assert(blobs.size() == testsize);					//DEBUG

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
			rectangle(colorImage, r.tl(), r.br(), Scalar(0, 0, 255), 2, 8, 0);
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

	return contours;
}

