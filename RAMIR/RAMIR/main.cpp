

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>

#include <opencv2\core.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\background_segm.hpp>

using namespace cv;
using namespace std;

Ptr<BackgroundSubtractor> pMOG;

ofstream ofile;
ifstream ifile;

int A = 500;
int B = 160;
int C = 3;
int D = 3;
int E = 200;
int F = 100;

int slider1=A, slider2=B, slider3=C, slider4=D, slider5=E, slider6=F;

Mat model;

void setHist(int value, void* a)
{
	A = value + 1;
	pMOG.release();
	pMOG = createBackgroundSubtractorMOG2(A, (((double)B) / 10), false);
}

void setBgThres(int value, void* a)
{
	B = value + 1;
	pMOG.release();
	pMOG = createBackgroundSubtractorMOG2(A, (((double)B) / 10), false);
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


void loadSettings()
{
	ifile.open("Settings.txt");

	if (!ifile.is_open())
	{
		exit(-1);
	}

	char t[100];
	ifile >> t;
	istringstream(t) >> A;
	ifile >> t;
	istringstream(t) >> B;
	ifile >> t;
	istringstream(t) >> C;
	ifile >> t;
	istringstream(t) >> D;
	ifile >> t;
	istringstream(t) >> E;
	ifile >> t;
	istringstream(t) >> F;
}


void find(Mat src);

int main()
{
	Mat frame, bgsub, erode, dilate, gray, temp;

	loadSettings();

	String video = "videoplayback1.mp4";
	VideoCapture cap(video);

	if (!cap.isOpened())
		return -1;

	namedWindow("Window", WINDOW_AUTOSIZE);
	namedWindow("Test", WINDOW_AUTOSIZE);
	namedWindow("Settings", WINDOW_NORMAL);

	resizeWindow("Settings", 560, 400);

	createTrackbar("BgHistory", "Settings", &slider1, 1000, setHist);
	createTrackbar("BgThreshold", "Settings", &slider2, 400, setBgThres);
	createTrackbar("ErodeElem", "Settings", &slider3, 60, setErodeElement);
	createTrackbar("DilateElem", "Settings", &slider4, 60, setDilateElement);
	createTrackbar("SizeX", "Settings", &slider5, 20000, setSizeX);
	createTrackbar("SizeY", "Settings", &slider6, 400, setSizeY);

	setTrackbarPos("BgHistory", "Settings", A);
	setTrackbarPos("BgThreshold", "Settings", B);
	setTrackbarPos("ErodeElem", "Settings", C);
	setTrackbarPos("DilateElem", "Settings", D);
	setTrackbarPos("SizeX", "Settings", E);
	setTrackbarPos("SizeY", "Settings", F);

	pMOG = createBackgroundSubtractorMOG2(A, (((double)B) / 10), false);
	
	cap >> frame;
	resize(frame, frame, Size(400, 300));

	cvtColor(frame, model, CV_BGR2GRAY);

	while (true)
	{
		cap >> frame;
		if (frame.empty())
		{
			cap.release();
			cap.open(video);
			cap >> frame;
		}

		pMOG->apply(frame, bgsub);

		cv::erode(bgsub, erode, erodeElement);

		cv::dilate(erode, dilate, dilateElement);
	
		/*Mat elli(F*2,E*2, CV_8UC1, 0.0);
		ellipse(elli, Point(E, F), Size(E, F), 0, 0, 360, Scalar(255),CV_FILLED, 8);

		matchTemplate(dilate, elli, temp, TM_CCORR);
		normalize(temp, temp, 0, 1, NORM_MINMAX, -1, Mat());

		double min, max;
		Point minLoc; 
		Point maxLoc;
		minMaxLoc(temp, &min, &max, &minLoc, &maxLoc, Mat());

		cout << min << "\t" << max << "\t" << minLoc << "\t" << maxLoc << endl;

		imshow("Test", temp);*/

		imshow("Window", frame);

		find(dilate);

		switch (waitKey(1))
		{
		case 27:
			cap.release();
			destroyAllWindows();
			
			ofile.open("Settings.txt");

			ofile << A << "\n";
			ofile << B << "\n";
			ofile << C << "\n";
			ofile << D << "\n";
			ofile << E << "\n";
			ofile << F << "\n";

			ofile.flush();
			ofile.close();

			return 0;
			break;
		case 13:
			reset(cap);
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
		if (moments(contours[i], false).m00 >= E)
			mu.push_back(moments(contours[i], false));
	}


	vector<Point2f> mc;
	for (Moments m : mu)
	{
		cout << m.m00 << endl;
		mc.push_back(Point2f(m.m10 / m.m00, m.m01 / m.m00));
	}


	Mat temp;
	cvtColor(src, temp, CV_GRAY2BGR, 3);
	for (Point2f p : mc)
	{
		ellipse(temp, p, Size(40, 40), 0, 0, 360, Scalar(0, 255, 0), 1);
	}


	namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	imshow("Contours", temp);
}