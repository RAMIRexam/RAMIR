#include "Blob.hpp"



/****************   C O N S T R U C T O R S   ***************/
Blob::Blob(Mat hist, Rect rect, Mat ROI) {
	setHist(hist);
	setRect(rect);
	setROI(ROI);
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

