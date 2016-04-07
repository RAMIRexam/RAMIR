/*
A blob is an object in the scene, probably a human. A blob is a vector of points that
defines where the blob is.

________________________________
|				|				|
|  ▓▓			|				|
|  ▓▓▓<--blob   |				|
|   ▓			|				|
|				|				|
|_______________|_______________|

*/

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

	Blob(Mat hist, Rect rect, Mat ROI, Point2f cent);
	Blob();							//create emptyblob
	~Blob();

	Rect getRect();
	void setRect(Rect rect);
	Mat getROI();
	void setROI(Mat ROI);
	Mat getHist();
	void setHist(Mat hist);
	Point2f getCent();
	void setCent(Point2f cent);
	


	bool emptyBlob;							//true if this is a emptyblob used for filling the blobvector in tracking

private:
	Mat hist;
	Mat ROI;
	Rect rect;
	Point2f centroid;


};

#endif
