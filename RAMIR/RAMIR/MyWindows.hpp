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
	MyWindows::MyWindows();
	~MyWindows();


private:
	void addWindow(String name);
	void showWindows();
	
	vector<String> windows;


};

#endif
