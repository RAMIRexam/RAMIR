#pragma once
#ifndef MYWINDOWS_HPP
#define MYWINDOWS_HPP

#include <stdio.h>
#include <stdlib.h>

#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\background_segm.hpp>


using namespace cv;
using namespace std;

class MyWindows {

public:
	MyWindows(int windowWidth);
	~MyWindows();

	static void feedImages(String name, Mat image);
	static void showImages();

private:
	

	static vector<Mat> images;
	static vector<String> imageNames;

	static int imageHeight;
	static int imageWidth;

	static int windowWidth;


};

#endif
