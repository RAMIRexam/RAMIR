#include "Blob.hpp"



/****************   C O N S T R U C T O R S   ***************/
Blob::Blob(Mat hist, Rect rect, Mat ROI) {
	setHist(hist);
	setRect(rect);
	setROI(ROI);
}
Blob::Blob() {
	emptyBlob = true;
}

Blob::~Blob() {

}
/************************************************************/



Mat Blob::getHist() {

}
void Blob::setHist(Mat hist) {

}


Rect Blob::getRect() {

}
void Blob::setRect(Rect Rect) {

}


Mat Blob::getROI() {

}
void Blob::setROI(Mat ROI) {

}

