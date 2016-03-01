#pragma once
#ifndef TEST_HPP
#define TEST_HPP

#include <fstream>
#include <sstream>
#include "Tracker.hpp"

#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\background_segm.hpp>

using namespace cv;

class Test {

public:

	static void trackerTest(Tracker t);

private:

};

#endif
