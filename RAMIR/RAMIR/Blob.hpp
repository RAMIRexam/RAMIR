#pragma once
#ifndef BLOB_HPP
#define BLOB_HPP

#include <fstream>
#include <sstream>

#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\background_segm.hpp>

using namespace cv;

class Blob {

public:

	Blob(Mat hist, Mat ROI, Rect rect);
	~Blob();

	Rect getRect();
	void setRect();
	Mat getROI();
	void setROI();
	Mat getHist();
	void setHist();

private:
	Mat hist;
	Mat ROI;
	Rect rect;


};

#endif
