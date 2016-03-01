#include "Tracker.hpp"



Tracker::Tracker(Rect rect) {
	numberTracks = 0;
	lastRect = rect;
	processed = false;
}

Tracker::~Tracker() {
	//cleanup
}

void Tracker::incrementTracks() {
	numberTracks++;
}

Rect Tracker::getLastRect() {
	return lastRect;
}

void Tracker::setLastRect(Rect r) {
	lastRect = r;
}

Mat Tracker::getLastHist() {
	return lastHist;
}

void Tracker::setLastHist(Mat h) {
	Tracker::lastHist = h;
}


void Tracker::fillWithBlob(Blob b) {
	blobvector.push_back(b);
}

/*
Fills the tracker with an empty blob (if no match was found)
*/
void Tracker::fillWithEmptyBlob() {
	Blob *b = new Blob();
	blobvector.push_back(*b);
}



/*
If survivalTest returns false the tracker shall be destroyed
*/
bool Tracker::survivalTest() {
	if (Blobvector.front().emptyBlob == true) { // if no blob could me matched with the tracker last iteration
		return false;
	}
	return true;
}

