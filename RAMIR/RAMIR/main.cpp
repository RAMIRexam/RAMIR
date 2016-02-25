//RAMIR

#include <stdlib.h>
#include <iostream>
#include <Windows.h>
#include "Settings.hpp"

#include <opencv2\core.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\background_segm.hpp>

using namespace cv;
using namespace std;

Ptr<BackgroundSubtractor> pMOG;

void find(Mat src);

int main()
{
	Mat frame, bgsub, erode, dilate, gray, temp;

	Settings::loadSettings();

	String video = "videoplayback1.mp4";
	VideoCapture cap(video);

	if (!cap.isOpened())
		return -1;

	namedWindow("Window", WINDOW_AUTOSIZE);

	pMOG = createBackgroundSubtractorMOG2(Settings::getA(), (((double)Settings::getB()) / 10), false);
	
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

		pMOG->apply(frame, bgsub);

		cv::erode(bgsub, erode, Settings::getErodeElement());
		cv::dilate(erode, dilate, Settings::getDilateElement());

		imshow("Window", frame);

		find(dilate);

		switch (waitKey(1))
		{

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



RNG rng(12345);
void find(Mat src)
{
	Mat src_cpy;
	src.copyTo(src_cpy);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;


	findContours(src_cpy, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));


	vector<Moments> mu;
	for (unsigned int i = 0; i < contours.size(); i++)
	{
		if (moments(contours[i], false).m00 >= Settings::getE())
			mu.push_back(moments(contours[i], false));

	}


	vector<Point2f> mc;
	for (Moments m : mu)
	{
		mc.push_back(Point2f(m.m10 / m.m00, m.m01 / m.m00));
	}


	Mat temp;
	cvtColor(src, temp, CV_GRAY2BGR, 3);
	for (Point2f p : mc)
	{
		ellipse(temp, p, Size(40, 40), 0, 0, 360, Scalar(0, 255, 0), 1);
	}

	line(temp, Point(temp.cols / 2, 0), Point(temp.cols / 2, temp.rows), Scalar(255, 0, 0), 2);

	namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	imshow("Contours", temp);
}