#include "Tracker.hpp"
#include "Definitions.hpp"

/*
________________________________
|				|				|
|				|				|
|entrypos		|vertical		|exitPos
|				|eeline			|
|				|pos			|
|_______________|_______________|

*/
Tracker::Tracker(Blob blob, int blobCenter, int entryPos, int exitPos) {
	fillWithBlob(blob);
	duration = 1;
	processed = true;
	fullTrackerLife = 2;
	trackerLife = fullTrackerLife;

	int lEntry = blobCenter - entryPos;					//Length to entry
	int lExit = exitPos - blobCenter;					//Length to exit

	assert(lEntry > 0 && lExit > 0);

	if (lEntry < lExit) { startDoor = ENTRY_DOOR; }	//Used to 
	else				{ startDoor = EXIT_DOOR; }

}

Tracker::~Tracker() {
	
	/*
	for (Blob b : blobvector) {
		blobvector
		delete &b;
	}
	*/

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

