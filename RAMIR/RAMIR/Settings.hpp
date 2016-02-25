#pragma once
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <fstream>
#include <sstream>

#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\background_segm.hpp>

class Settings {

	public:
		static void init(cv::Ptr<cv::BackgroundSubtractor> *pMOG);

		static void setHist(int value, void* a);
		static void setBgThres(int value, void* a);
		static void setErodeElement(int value, void* a);
		static void setDilateElement(int value, void* a);
		static void setAreaThres(int value, void *a);

		static void saveSettings();
		static void loadSettings();

		static int getA();
		static int getB();
		static int getC();
		static int getD();
		static int getE();

		static cv::Mat getErodeElement();
		static cv::Mat getDilateElement();

	private:
		static int A, B, C, D, E;
		static int slider1, slider2, slider3, slider4, slider5;
		static cv::Mat erodeElement, dilateElement;
		static std::ofstream ofile;
		static std::ifstream ifile;
		
};

#endif // !SETTINGS_HPP
