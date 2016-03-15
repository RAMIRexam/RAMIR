#include "MyWindows.hpp"
/*
(1) Feed images with feedImage()
(2) Show images with showImages() - this deletes the fed images

*/


/****************   C O N S T R U C T O R S   ***************/
MyWindows::MyWindows(int windowWidth) {
	imageWidth = 530;		//640
	imageHeight = 600;		//480

	//windowWidth = 1500;//?
}

MyWindows::~MyWindows() {

}
/************************************************************/



void MyWindows::feedImages(String name, Mat image) {
	imageNames.push_back(name);
	images.push_back(image);

}

void MyWindows::showImages() {

	assert(images.size() > 0);
	assert(images.size() == imageNames.size());
	

	//int temp_imageWidth = windowWidth / images.size();
	//if (temp_imageWidth < 600) {
	//	imageWidth = temp_imageWidth;
	//}


	int widthCounter = 0;

	while (images.size() > 0) {
		String name = imageNames.back();
		Mat image = images.back();

		images.pop_back();
		imageNames.pop_back();


		imshow(name, image);
		resizeWindow(name, imageWidth, imageHeight);
		moveWindow(name, widthCounter*imageWidth, 0);
		
		widthCounter++;
	}
}
