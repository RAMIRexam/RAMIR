#pragma once
#ifndef TRACKER_HPP
#define TRACKER_HPP

#include <fstream>
#include <sstream>
#include <iostream>
#include "Blob.hpp"

#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\background_segm.hpp>

using namespace cv;
using namespace std;

class Tracker {

public:

	Tracker(Blob blob);
	~Tracker();

	int getDuration();
	Blob getLastBlob();
	bool survivalTest();
	void fillWithBlob(Blob b);
	void fillWithEmptyBlob();
	void printTracker();


	bool processed;					//true if the tracker is processed this iteration


private:
	vector<Blob> blobvector;		//contains all detected blobs in order
	int duration;
	int fullTrackerLife;
	int trackerLife;
};

#endif // !SETTINGS_HPP
