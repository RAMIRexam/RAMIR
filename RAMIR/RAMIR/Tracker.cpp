#include "Tracker.hpp"


Tracker::Tracker(Rect rect) {
	numberTracks = 0;
	lastRect = rect;
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

