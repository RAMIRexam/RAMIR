#include "Tracker.hpp"

/*
_________________________________
|				|				 |
|				|				 |
|LEFTSIDE_OFLINE|RIGHTSIDE_OFLINE|
|				|				 |
|				|				 |
|_______________|________________|

*/
Tracker::Tracker(Blob blob, Scene scene) {
	fillWithBlob(blob);
	duration = 1;
	processed = true;
	fullTrackerLife = 2;
	trackerLife = fullTrackerLife;


	// Start side of entry/exit-line
	/******************************************/
	int lineSide = scene.LSCheck(blob);

	if (lineSide == LEFTSIDE_OFLINE) {
		staSOL = LEFTSIDE_OFLINE;
	}
	else {
		assert(lineSide == RIGHTSIDE_OFLINE);
		staSOL = RIGHTSIDE_OFLINE;
	}
	/******************************************/
}

Tracker::~Tracker() {
	
	/*
	for (Blob b : blobvector) {
		blobvector
		delete &b;
	}
	*/

}

//Closest door check
void Tracker::CDCheck(Blob b) {

}

int Tracker::getDuration() {
	return duration;
}

Blob Tracker::getLastBlob() {
	assert(blobvector.size() > 0);
	return blobvector.back();
}


void Tracker::fillWithBlob(Blob b) {
	blobvector.push_back(b);
	duration++;
}

/*
Fills the tracker with an empty blob (if no match was found)
*/
void Tracker::fillWithEmptyBlob() {
	Blob *b = new Blob();
	blobvector.push_back(*b);
	duration++;
}

void Tracker::printTracker() {
	
	getLastBlob();

	//cout << "lastblob: " << getLastBlob() << "\t lasthist " << getLastHist() << endl;

}



/*
If survivalTest returns false the tracker shall be destroyed
*/
bool Tracker::survivalTest() {
	assert(blobvector.size() != 0);

	if (blobvector.back().emptyBlob == true) { // if no blob could me matched with the tracker last iteration
		trackerLife--;
		
		if (trackerLife == 0) { 
			return false; 
		}
	}
	else { trackerLife = fullTrackerLife; }
	

	return true;
}

