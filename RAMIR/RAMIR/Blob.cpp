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

#include "Blob.hpp"



/****************   C O N S T R U C T O R S   ***************/
Blob::Blob(Mat hist, Rect rect, Mat ROI, Point2f cent) {
	setHist(hist);
	setRect(rect);
	setROI(ROI);
	setCent(cent);

	emptyBlob = false;
}
Blob::Blob() {
	emptyBlob = true;
}

Blob::~Blob() {

}
/************************************************************/



Mat Blob::getHist() {
	return hist;
}
void Blob::setHist(Mat h) {
	hist = h;
}


Rect Blob::getRect() {
	return rect;
}
void Blob::setRect(Rect r) {
	rect = r;
}


Mat Blob::getROI() {
	return ROI;
}
void Blob::setROI(Mat roi) {
	ROI = roi;
}

Point2f Blob::getCent() {
	return centroid;
}

void Blob::setCent(Point2f cent) {
	centroid = cent;
}
