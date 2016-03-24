/*
class used to create edit images
*/


#include "mergeImages.hpp"

editImages::editImages();

/**************************************************************************************************************************************
/	Following code was used to create an image demonstrating the first program.
**************************************************************************************************************************************/
editImages::mergeImages() {
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

