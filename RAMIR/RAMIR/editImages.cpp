/*
class used to create edit images
*/

#define EDITIMAGES
#ifdef EDITIMAGES


//#include "mergeImages.hpp"
#include "editImages.hpp"


using namespace std;
using namespace cv;

editImages::editImages() {

}

void editImages::morphological() {

	int imageSize = 12;
	int kernelSize = 3;

	Mat M = (Mat_<unsigned char>(imageSize, imageSize) <<
		
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
		0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
		0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0,
		0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0,
		0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0,
		0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	//Mat S = Mat::ones(3, 3, CV_16U);

	Mat S = getStructuringElement(MORPH_RECT, Size(kernelSize, kernelSize));


	Mat eroded;
	Mat dilated;

	erode(M, eroded, S);
	dilate(M, dilated, S);


	Mat kernelBackground = Mat::zeros(imageSize, imageSize, CV_32FC1);
	S.convertTo(S, CV_32FC1);


	Rect r = Rect(kernelBackground.cols / 2 - (kernelSize/2), kernelBackground.rows / 2 - (kernelSize / 2), kernelSize, kernelSize);
	Mat kernelROI = kernelBackground(r);
	kernelROI = kernelROI | S;
	
	M = resizeImage(M, 0.05);
	eroded = resizeImage(eroded, 0.05);
	dilated = resizeImage(dilated, 0.05);

	threshold(M, M, 0, 255, THRESH_BINARY);							//temp_bgsub will contain 0 or 255 values
	threshold(eroded, eroded, 0, 255, THRESH_BINARY);							//temp_bgsub will contain 0 or 255 values
	threshold(dilated, dilated, 0, 255, THRESH_BINARY);							//temp_bgsub will contain 0 or 255 values

	imwrite("C:\\Users\\Emil\\Desktop\\PROJEKT\\Examensarbete\\Bilder\\Morfologi\\erodedDemonstration.jpg", eroded);
	imwrite("C:\\Users\\Emil\\Desktop\\PROJEKT\\Examensarbete\\Bilder\\Morfologi\\dilatedDemonstration.jpg", dilated);
	imwrite("C:\\Users\\Emil\\Desktop\\PROJEKT\\Examensarbete\\Bilder\\Morfologi\\beforeMorfDemonstration.jpg", M);


	M.convertTo(M, CV_32FC1);
	eroded.convertTo(eroded, CV_32FC1);		//This makes ones to turn into white pixels in "image watch"
	dilated.convertTo(dilated, CV_32FC1);	//This makes ones to turn into white pixels in "image watch"

	


	getchar();
}

Mat editImages::resizeImage(Mat frame, double divider) {
	Mat out;																//Resize the image													//
																			//
	int width = frame.cols / divider;										//
	int height = frame.rows / divider;										//
																			//
	Size size(width, height);												//
	resize(frame, out, size, 0, 0, CV_INTER_AREA);												//
	return out;


	/*
	For resize

	INTER_NEAREST - a nearest-neighbor interpolation
	INTER_LINEAR - a bilinear interpolation (used by default)
	INTER_AREA - resampling using pixel area relation. It may be a preferred method for image decimation, as it gives moire’-free results. But when the image is zoomed, it is similar to the INTER_NEAREST method.
	INTER_CUBIC - a bicubic interpolation over 4x4 pixel neighborhood
	INTER_LANCZOS4 - a Lanczos interpolation over 8x8 pixel 
	*/

}

/**************************************************************************************************************************************
/	Following code was used to create an image demonstrating the first program.
**************************************************************************************************************************************/
void editImages::mergeImages() {
	vector<Mat> MI;
	vector<Mat> MU;
	vector<Mat> Last;

	Mat mat1;
	Mat mat2;
	Mat mat3;
	Mat mat4;
	Mat mat5;
	Mat mat6;

	Mat merge1;
	Mat merge2;
	Mat result;

	mat1 = imread("C:\\Users\\Emil\\Desktop\\PROJEKT\\Examensarbete\\Bilder\\Del 1 Utbildningsfas\\Tracking images\\tracking28.jpg");
	mat2 = imread("C:\\Users\\Emil\\Desktop\\PROJEKT\\Examensarbete\\Bilder\\Del 1 Utbildningsfas\\Tracking images\\tracking30.jpg");
	mat3 = imread("C:\\Users\\Emil\\Desktop\\PROJEKT\\Examensarbete\\Bilder\\Del 1 Utbildningsfas\\Tracking images\\tracking32.jpg");
	mat4 = imread("C:\\Users\\Emil\\Desktop\\PROJEKT\\Examensarbete\\Bilder\\Del 1 Utbildningsfas\\Tracking images\\tracking34.jpg");
	mat5 = imread("C:\\Users\\Emil\\Desktop\\PROJEKT\\Examensarbete\\Bilder\\Del 1 Utbildningsfas\\Tracking images\\tracking36.jpg");
	mat6 = imread("C:\\Users\\Emil\\Desktop\\PROJEKT\\Examensarbete\\Bilder\\Del 1 Utbildningsfas\\Tracking images\\tracking38.jpg");

	putText(mat1, "1", Point(mat1.cols - 100, 100), FONT_HERSHEY_COMPLEX, 4, Scalar(0, 0, 255), 4);
	putText(mat2, "2", Point(mat1.cols - 100, 100), FONT_HERSHEY_COMPLEX, 4, Scalar(0, 0, 255), 4);
	putText(mat3, "3", Point(mat1.cols - 100, 100), FONT_HERSHEY_COMPLEX, 4, Scalar(0, 0, 255), 4);
	putText(mat4, "4", Point(mat1.cols - 100, 100), FONT_HERSHEY_COMPLEX, 4, Scalar(0, 0, 255), 4);
	putText(mat5, "5", Point(mat1.cols - 100, 100), FONT_HERSHEY_COMPLEX, 4, Scalar(0, 0, 255), 4);
	putText(mat6, "6", Point(mat1.cols - 100, 100), FONT_HERSHEY_COMPLEX, 4, Scalar(0, 0, 255), 4);




	Mat blackBorder_hor = Mat::zeros(Size(10, mat1.rows), CV_8UC3);


	MI.push_back(mat1);
	MI.push_back(blackBorder_hor);
	MI.push_back(mat2);
	MI.push_back(blackBorder_hor);
	MI.push_back(mat3);

	MU.push_back(mat4);
	MU.push_back(blackBorder_hor);
	MU.push_back(mat5);
	MU.push_back(blackBorder_hor);
	MU.push_back(mat6);




	hconcat(MI, merge1);
	hconcat(MU, merge2);

	Mat blackBorder_ver = Mat::zeros(Size(merge1.cols, 10), CV_8UC3);


	Last.push_back(merge1);
	Last.push_back(blackBorder_ver);
	Last.push_back(merge2);

	vconcat(Last, result);

	imwrite("C:\\Users\\Emil\\Desktop\\PROJEKT\\Examensarbete\\Bilder\\Del 1 Utbildningsfas\\Tracking images\\TrackingDemonstration.jpg", result);
}

#endif // !EDITIMAGES