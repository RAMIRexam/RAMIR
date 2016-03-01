//RAMIR

#include <stdlib.h>
#include <iostream>
#include <Windows.h>
#include "Settings.hpp"
#include "Tracker.hpp"

#include <opencv2\core.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\background_segm.hpp>

using namespace cv;
using namespace std;

Ptr<BackgroundSubtractor> pMOG;
Mat colorImage;


vector<Tracker> createTrackers(vector<Rect> rects);
void find(Mat binImage, vector<vector<Point>> contours);
vector<vector<Point>> myFindContours(Mat src);
vector<Rect> getAllRects(vector<vector<Point>> contours);


vector<Tracker> trackers;
vector<Tracker> alreadyCountedTrackers;

bool paintRect = true;


int main()
{
	Mat frame, prevBG, bgsub, erode, dilate, gray, temp, hist1, hist2, lastHist, tempHist, lastROI;
	Rect lastRect;

	Settings::loadSettings();

	String video = "videoplayback2.mp4";
	VideoCapture cap(video);

	if (!cap.isOpened())
		return -1;

	namedWindow("Window", WINDOW_AUTOSIZE);

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

		cv::erode(bgsub, erode, Settings::getErodeElement());
		cv::dilate(erode, dilate, Settings::getDilateElement());

		imshow("Window", frame);

		vector<vector<Point>> contours = myFindContours(dilate);


		if (contours.size() > 0) {
			//marks the centroid
			find(dilate, contours);


			//---------------GET INTERSECTING RECTS------------------------------
			
			vector<Rect> interRects; //Rectangles that intersect with the old one

			vector<Rect> allRects = getAllRects(contours);
			vector<Tracker> newTrackers = createTrackers(allRects);
			
			//first time
			if (trackers.size() == 0) {
				
				while (newTrackers.size() > 0) {
					trackers.push_back(newTrackers.back());		//move the tracker
					newTrackers.pop_back();						//delete the tracker
				}

				//calc hist for all trackers---
				//??
				//-------------------------------
				
			}
			else {

				int trackercount = 0; //for debugging
				
				for (Tracker t : trackers) {
					trackercount++;
					for (Rect r : allRects) {

						//check whether the rectangles intersect , ("&" = "snittet")
						if ((r & t.getLastRect()).area() > 0) {
							interRects.push_back(r);
						}
					}
					cout << "tracker: " << trackercount << "\t intersecting rects: " << interRects.size() << endl;
					//--------------------------------------------------------------------



					//---------------CALC ROI AND HIST------------------------------------
					vector<Mat> interROIs; //intersecting Region of interest's

					vector<Mat> hists;
					Mat bestROI;
					double minBhatta = DBL_MAX;

					for (Rect r : interRects) {
						//interROIs.push_back(frame(r));
						Mat ROI = frame(r);
						calcHist(&ROI, 1, channels, Mat(), tempHist, 2, histSize, ranges);
						//hists.push_back(tempHist);
						createHist(channels, histSize, )
						

						double bhatta = compareHist(tempHist, t.getLastHist(), CV_COMP_BHATTACHARYYA);

						if (bhatta < minBhatta) {
							minBhatta = bhatta;
							bestROI = ROI;
							
							//update tracker
							t.setLastHist(tempHist);
							t.setLastRect(r);
						}
					}
					//--------------------------------------------------------------------
					
					//---------------CALCULATE HISTOGRAM----------------------------------
					//--------------------TEST----------------------
					//calcHist(&bgsub, 1, channels, Mat(), hist1, 2, histSize, ranges);
					//calcHist(&prevBG, 1, channels, Mat(), hist2, 2, histSize, ranges);
					//calcHist(&bgsub, 1, 0, Mat(), hist1, 1, histSize, ranges);
					//calcHist(&prevBG, 1, 0, Mat(), hist2, 1, histSize, ranges);
					//--------------------------------------------------------------------
				}
			}
		}

		//countors == 0
		else {
			//delete tracker
			while (trackers.size() > 0) {
				trackers.pop_back();
			}

		}


		for (Tracker t : trackers) {
			if (!t.survivalTest())
				t.~Tracker();
		}




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
Creates new trackers from given rectangles
*/
vector<Tracker> createTrackers(vector<Rect> rects) {
	vector<Tracker> newTrackers;
	
	for (Rect r : rects) {
		newTrackers.push_back(Tracker(r));
	}

	return newTrackers;
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

	namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	imshow("Contours", colorImage);

}


//--------------------PSEUDOKOD TRACKING----------------

while (true)
{
	//CAPTURE IMAGE
	//BGSUB
	//DILATE
	//GET ALL CONTOURS FROM DILATE

	
	//getallrects
	//create blobs for all rects

	if (trackers == 0) { - kan inte jämföra med några trackers...
		//create trackers for all blobs
		//candidate trackers >> trackers
	}
		
	else { //- there is 2 outcomes - the 2 outcomes -> match with tracker OR new tracker

		for (Tracker t : trackers) {
			//init intersecting rect variables
			for (Blob b : allBlobs) {
				
				/*
				(1)  -match with tracker?
				*/
				if (blob intersect with tracker) {
					//calculate and update the one with least bhattacharryan coeff
					//throw other intersecting rects - (will be done automaticly)
				}
				/*
				(2)  -new tracker
				*/
				else {
					//create new tracker
					//create new blob (calc ROI and hist)
					//tracker = processed
				}
			}

			/*
				If a rect match the tracker
			*/
			if (a blob intersect with the tracker) {
				//create new blob for intersecting rect (calc ROI and hist)
				//fill the tracker with the new blob
				//tracker.processed = true
			}
			//the tracker has no intersecting blobs
			else {
				//t.fillWithEmptyBlob();
				//tracker.processed = true
			}
			
			
			
		}

		for (Tracker t : trackers) {
			if (tracker != processed) {
				//t.fillWithEmptyBlob();
				//tracker.processed = true;
			}
		}
	}
	



	Test::trackerTest(trackers); //assertiontest for trackers

	//check if trackers shall survive
	for (Tracker t : trackers) {
		if (t.survivaltest()) {
			t.~tracker();
		}
	}

	//reset processed for next iteration
	for (Trackers t : trackers) {
		t.processed = false;
	}
}


/*
if more than one rect intersect with a trackerobject, only one shall be used, the others shall be tossed.


*/