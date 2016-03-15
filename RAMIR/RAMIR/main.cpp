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
Mat colorImage;


void find(Mat binImage, vector<vector<Point>> contours);
vector<vector<Point>> myFindContours(Mat src);
vector<Rect> getAllRects(vector<vector<Point>> contours);
vector<Blob> createBlobs(vector<Rect> rects, Mat image);

vector<Tracker*> trackers;
vector<Tracker> alreadyCountedTrackers;

bool paintRect = true;


int main()
{
	Mat frame, prevBG, bgsub, erode, dilate, gray, temp, hist1, hist2, lastHist, tempHist, lastROI;
	Rect lastRect;

	Settings::loadSettings();

	MyWindows windows(1500);

	String video = "videoplayback2.mp4";
	VideoCapture cap(video);

	//cap.set(CV_CAP_PROP_FRAME_WIDTH, imageWidth);		Fick ej att fungera, använder resizeWindow() istället
	//cap.set(CV_CAP_PROP_FRAME_HEIGHT, imageHeight);

	if (!cap.isOpened())
		return -1;


	namedWindow("Window", WINDOW_AUTOSIZE);
	namedWindow("BGS", WINDOW_AUTOSIZE);
	namedWindow("Contours", CV_WINDOW_AUTOSIZE);

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

	while (true)
	{	
		
		cap >> frame;

		

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

		
		windows.feedImages("Windows", frame);
		windows.feedImages("Dilate Window", dilate);


		vector<vector<Point>> contours = myFindContours(dilate);
		vector<Rect> allRects = getAllRects(contours);
		vector<Blob> blobs = createBlobs(allRects, frame);

		//finds the centroid
		find(dilate, contours);
		
		if (trackers.size() == 0) {
			/*
			No trackers exists. All blobs will turn to a tracker
			*/
			for (Blob b : blobs) {
				Tracker *t = new Tracker(b);

				cout << t << endl;

				t->processed = true;
				trackers.push_back(t);

			}
		}

		//there already exists trackers
		else {
			for (Tracker *t : trackers) {
				vector<Blob> restBlobs;
				Blob bestBlob;				// constructs an emptyblob
				assert(bestBlob.emptyBlob == true);	//if not change if below

				double minBhatta = DBL_MAX;
				vector<Mat> isBlobs;			//InterSecting Blobs
				
				int blobcounter = 0; //DEBUGGING!
				int numberBlobs = blobs.size();

				while (blobs.size() > 0) {
					blobcounter++;//DEBUGGING
					
					Blob b = blobs.back();
					//Pop makes shure other intersecting blobs than the best is ignored
					blobs.pop_back();

					//intersection test
					if ((b.getRect() & t->getLastBlob().getRect()).area() > 0) {
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
				assert(blobcounter == numberBlobs); //if blob is pop'ed, will all blobs still be iterated?
				
				//Debugvariable
				int testsize = restBlobs.size();
				
				blobs = restBlobs; //memcpy(&blobs, &restBlobs, sizeof(restBlobs));

				/*DEBUG*/
				assert(&blobs != &restBlobs);			
				assert(blobs.size() == testsize);
				/*DEBUG*/

				//If intersection
				if (!bestBlob.emptyBlob) { t->fillWithBlob(bestBlob); }
				else { t->fillWithEmptyBlob(); }
					
				t->processed = true;
	
			}
			
			//iterate throught the rest of the blobs and create trackers for them
			for (Blob b : blobs) {
				Tracker *t = new Tracker(b);
				t->processed = true;
				trackers.push_back(t);
				
			}

			for (Tracker *t : trackers) {
				if (!t->processed) { 
					t->fillWithEmptyBlob(); 
					t->processed = true;

					cout << "Tracker: " << t << endl;

				}
			}
		}
		
		

		for (Tracker *t : trackers) {
			cout << "Tracker: " << t << endl;

			assert(t->processed == true);
		}

		//DEBUGGING
		int numbertrackers = trackers.size();
		int counter = 0;

		//check if trackers shall survive
		vector<Tracker*> tempTrackers;

		while (trackers.size() > 0) {
			counter++;

			Tracker *t = trackers.back();

			trackers.pop_back();
			t->processed = false;
			
			if (t->survivalTest()) { 
				tempTrackers.push_back(t); 
				cout << t << endl;
			}
			else {
				delete t;
			}
			

		}

		trackers = tempTrackers;

		assert(trackers.size() == tempTrackers.size());
		assert(counter == numbertrackers);


		for (Tracker *t : trackers) {
			assert(t->processed == false);
		}


		/*
		Paint trackinginfo
		*/
		int dText = 20;
		int trackername = 1;

		int minDuration = 10;
		int paintRectDuration = 15;

		
		for (Tracker *t : trackers) {
			if (t->getDuration() >= minDuration) {
				putText(colorImage, "Tracker " + to_string(trackername) + ": Number detections " + to_string(t->getDuration()), Point(20, 20 + dText), FONT_HERSHEY_DUPLEX, 0.5, Scalar(0, 255, 0), 1);
				dText += 20;
				trackername++;
				
				if (t->getDuration() > paintRectDuration) {
					rectangle(colorImage, t->getLastBlob().getRect().tl(), t->getLastBlob().getRect().br(), Scalar(0, 0, 255), 2, 8, 0);
				}
			}

			
		}
		

		windows.feedImages("Contours", colorImage);
		windows.showImages();



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

/*
Creates blobs from rectangles
*/
vector<Blob> createBlobs(vector<Rect> rects, Mat image) {
	//--------------TESTING--------------------------------
	int hbins = 30, sbins = 32;
	int histSize[] = { hbins, sbins };
	float hranges[] = { 0, 180 };
	float sranges[] = { 0, 256 };
	const float* ranges[] = { hranges, sranges };
	int channels[] = { 0, 1 };
	Mat tempHist;
	vector<Blob> tempBlobs;

	for (Rect r : rects) {
		Mat blobROI = image.clone();
		blobROI(r);


		calcHist(&blobROI, 1, channels, Mat(), tempHist, 2, histSize, ranges);

		
		//Niklas
		//calcHist(&bgsub, 1, channels, Mat(), hist1, 2, histSize, ranges);
		//calcHist(&prevBG, 1, channels, Mat(), hist2, 2, histSize, ranges);
		//calcHist(&bgsub, 1, 0, Mat(), hist1, 1, histSize, ranges);
		//calcHist(&prevBG, 1, 0, Mat(), hist2, 1, histSize, ranges);

		//Emil
		//calcHist(&test 1, channels, Mat(), tempHist, 2, histSize, ranges);

		Blob b(tempHist, r, blobROI);
		tempBlobs.push_back(b);
		

		imshow("debug hist", tempHist);

	}

	return tempBlobs;
}




/*
returns a vector containing all rectangles around each object.
*/
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


vector<vector<Point>> myFindContours(Mat src) {
	Mat src_cpy;
	src.copyTo(src_cpy);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours(src_cpy, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	return contours;
}


RNG rng(12345);
void find(Mat binImage, vector<vector<Point>> contours)
{

	cvtColor(binImage, colorImage, CV_GRAY2BGR, 3);

	vector<Moments> mu;
	vector<Rect> rects;
	for (unsigned int i = 0; i < contours.size(); i++)
	{
		if (moments(contours[i], false).m00 >= Settings::getE()) { //If contour is big enough
			mu.push_back(moments(contours[i], false));
			
		}
	}




	vector<Point2f> mc;
	for (Moments m : mu)
	{
		mc.push_back(Point2f(m.m10 / m.m00, m.m01 / m.m00));
	}



	
	for (Point2f p : mc)
	{
		ellipse(colorImage, p, Size(40, 40), 0, 0, 360, Scalar(0, 255, 0), 1);
	}

	line(colorImage, Point(colorImage.cols / 2, 0), Point(colorImage.cols / 2, colorImage.rows), Scalar(255, 0, 0), 2);


}


