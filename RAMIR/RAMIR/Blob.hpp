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

	Blob(Mat hist, Rect rect, Mat ROI);
	Blob();							//create emptyblob
	~Blob();

	Rect getRect();
	void setRect(Rect rect);
	Mat getROI();
	void setROI(Mat ROI);
	Mat getHist();
	void setHist(Mat hist);

	bool emptyBlob;							//true if this is a emptyblob used for filling the blobvector in tracking

private:
	Mat hist;
	Mat ROI;
	Rect rect;


};

#endif
