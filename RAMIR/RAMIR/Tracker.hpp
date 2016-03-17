#pragma once
#ifndef TRACKER_HPP
#define TRACKER_HPP

#include <fstream>
#include <sstream>
#include <iostream>
#include "Blob.hpp"
#include "Definitions.hpp"
#include "Scene.hpp"

#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\background_segm.hpp>

using namespace cv;
using namespace std;

class Tracker {

public:

	Tracker(Blob blob, Scene scene);
	~Tracker();

	int getDuration();
	Blob getLastBlob();
	bool survivalTest();
	void fillWithBlob(Blob b);
	void fillWithEmptyBlob();
	void printTracker();

	

	int curSOL;						//current Side Of Line, which side of the line the object currently is
	int staSOL;						//start Side Of Line, which side of the line the object was when it was first detected

	bool processed;					//true if the tracker is processed this iteration


private:
	vector<Blob> blobvector;		//contains all detected blobs in order
	int duration;
	int fullTrackerLife;
	int trackerLife;

	
};

#endif // !SETTINGS_HPP
