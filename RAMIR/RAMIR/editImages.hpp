
#ifndef EDITIMAGES_HPP
#define EDITIMAGES_HPP

#include <fstream>
#include <sstream>

#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\background_segm.hpp>

using namespace cv;

class editImages {

public:

	editImages();
	//~editImages();

	void mergeImages();
	static void morphological();
	static Mat resizeImage(Mat frame, double divider);

private:


};

#endif
