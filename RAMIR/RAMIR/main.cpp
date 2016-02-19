

#include <stdlib.h>
#include <iostream>
#include <Windows.h>

#include <opencv2\core.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\background_segm.hpp>

using namespace cv;
using namespace std;

//Ptr<BackgroundSubtractor> pMOG;

int A = 20;
int B = 0;
int C = 3;
int D = 3;
int E = 200;
int F = 100;

int slider1=A, slider2=B, slider3=C, slider4=D, slider5=E, slider6=F;

Mat model;

void setHist(int value, void* a)
{
	A = value + 1;
	//pMOG.release();
	//pMOG = createBackgroundSubtractorMOG2(A, (((double)B) / 10), true);
}

void setBgThres(int value, void* a)
{
	B = value + 1;
	//pMOG.release();
	//pMOG = createBackgroundSubtractorMOG2(A, (((double)B) / 10), true);
}

Mat erodeElement = getStructuringElement(MORPH_RECT, Size(C, C));
void setErodeElement(int value, void* a)
{
	C = value + 1;
	erodeElement = getStructuringElement(MORPH_RECT, Size(C, C));
}

Mat dilateElement = getStructuringElement(MORPH_RECT, Size(D, D));
void setDilateElement(int value, void* a)
{
	D = value + 1;
	dilateElement = getStructuringElement(MORPH_RECT, Size(D, D));
}

void setSizeX(int value, void *a)
{
	E = value + 1;
}

void setSizeY(int value, void *a)
{
	F = value + 1;
}


void reset(VideoCapture cap)
{
	cap >> model;
	cvtColor(model, model, CV_BGR2GRAY);
}

void find(Mat src);

int main()
{
	Mat frame, bgsub, erode, dilate, gray, temp;
	VideoCapture cap(0);

	if (!cap.isOpened())
		return -1;

	namedWindow("Window", WINDOW_AUTOSIZE);
	namedWindow("Test", WINDOW_AUTOSIZE);
	namedWindow("Settings", WINDOW_NORMAL);

	resizeWindow("Settings", 560, 400);

	createTrackbar("BgThreshold", "Settings", &slider1, 255, setHist);
	createTrackbar("---", "Settings", &slider2, 255, setBgThres);
	createTrackbar("ErodeElem", "Settings", &slider3, 40, setErodeElement);
	createTrackbar("DilateElem", "Settings", &slider4, 40, setDilateElement);
	createTrackbar("SizeX", "Settings", &slider5, 400, setSizeX);
	createTrackbar("SizeY", "Settings", &slider6, 400, setSizeY);

	
	//pMOG = createBackgroundSubtractorMOG2(A, (((double)B) / 10), true);
	
	cap >> model;
	cvtColor(model, model, CV_BGR2GRAY);

	while (true)
	{
		cap >> frame;

		//pMOG->apply(frame, bgsub);
		cvtColor(frame, gray, CV_BGR2GRAY);
		subtract(model, gray, temp);
		abs(temp);
		threshold(temp, bgsub, A, 255, THRESH_BINARY);

		cv::erode(bgsub, erode, erodeElement);

		cv::dilate(erode, dilate, dilateElement);
	
		Mat elli(F*2,E*2, CV_8UC1, 0.0);
		ellipse(elli, Point(E, F), Size(E, F), 0, 0, 360, Scalar(255),CV_FILLED, 8);

		matchTemplate(dilate, elli, temp, TM_CCORR);

		double min, max;
		minMaxLoc(temp, &min, &max);

		cout << min << "\t" << max << endl;

		imshow("Test", temp);
		imshow("Window", dilate);

		find(dilate);

		switch (waitKey(1))
		{
		case 27:
			return 0;
			break;
		case 13:
			reset(cap);
			break;
		}
	}

	destroyAllWindows();
	return 0;
}



RNG rng(12345);

void find(Mat src)
{
	Mat src_cpy;
	src.copyTo(src_cpy);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Find contours
	findContours(src_cpy, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Get the moments
	vector<Moments> mu(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		mu[i] = moments(contours[i], false);
	}

	///  Get the mass centers:
	vector<Point2f> mc(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	}


	Mat temp;
	cvtColor(src, temp, CV_GRAY2BGR, 3);
	for (int i = 0; i< contours.size(); i++)
	{
		circle(temp, mc[i], 10, Scalar(0, 255,0), 2);
	}

	/// Show in a window
	namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	imshow("Contours", temp);
}