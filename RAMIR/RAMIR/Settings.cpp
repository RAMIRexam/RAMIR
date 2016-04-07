#include "Settings.hpp"

int Settings::A = 500;
int Settings::B = 160;
int Settings::C = 3;
int Settings::D = 3;
int Settings::E = 4000;

int Settings::slider1 = A;
int Settings::slider2 = B;
int Settings::slider3 = C;
int Settings::slider4 = D;
int Settings::slider5 = E;

cv::Mat Settings::erodeElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(C, C));
cv::Mat Settings::dilateElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(D, D));

std::ofstream Settings::ofile;
std::ifstream Settings::ifile;





//-------------INIT WINDOW FOR TRACKBARS--------------------
void Settings::init(cv::Ptr<cv::BackgroundSubtractor> *pMOG)
{
	cv::namedWindow("Settings", cv::WINDOW_NORMAL);

	cv::resizeWindow("Settings", 560, 400);

	cv::createTrackbar("BgHistory", "Settings", &slider1, 1000, setHist, pMOG);
	cv::createTrackbar("BgThreshold", "Settings", &slider2, 400, setBgThres, pMOG);
	cv::createTrackbar("ErodeElem", "Settings", &slider3, 60, setErodeElement, pMOG);
	cv::createTrackbar("DilateElem", "Settings", &slider4, 60, setDilateElement, pMOG);
	cv::createTrackbar("AreaThres", "Settings", &slider5, 20000, setAreaThres, pMOG);

	cv::setTrackbarPos("BgHistory", "Settings", A);
	cv::setTrackbarPos("BgThreshold", "Settings", B);
	cv::setTrackbarPos("ErodeElem", "Settings", C);
	cv::setTrackbarPos("DilateElem", "Settings", D);
	cv::setTrackbarPos("AreaThres", "Settings", E);
}
//-------------------------------------------------------










//-----------TRACKBAR FUNCTIONS------------------------
void Settings::setHist(int value, void * a)
{
	A = value + 1;
	cv::Ptr<cv::BackgroundSubtractor> *pMOG = (cv::Ptr<cv::BackgroundSubtractor> *)a;
	pMOG->release();
	*pMOG = cv::createBackgroundSubtractorMOG2(A, (((double)B) / 10), false);
}

void Settings::setBgThres(int value, void * a)
{
	B = value + 1;
	cv::Ptr<cv::BackgroundSubtractor> *pMOG = (cv::Ptr<cv::BackgroundSubtractor> *)a;
	pMOG->release();
	*pMOG = cv::createBackgroundSubtractorMOG2(A, (((double)B) / 10), false);
}

void Settings::setErodeElement(int value, void * a)
{
	C = value + 1;
	erodeElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(C, C));
}

void Settings::setDilateElement(int value, void * a)
{
	D = value + 1;
	dilateElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(D, D));
}

void Settings::setAreaThres(int value, void * a)
{
	E = value + 1;
}
//-----------------------------------------------









//------------LOAD AND SAVE FUNCTIONS------------------
void Settings::saveSettings()
{
	ofile.open("Settings.txt");

	ofile << A-1 << "\n";
	ofile << B-1 << "\n";
	ofile << C-1 << "\n";
	ofile << D-1 << "\n";
	ofile << E-1 << "\n";

	ofile.flush();
	ofile.close();
}


void Settings::loadSettings()
{
	ifile.open("Settings.txt");

	if (ifile.is_open())
	{
		char t[100];
		ifile >> t;
		std::istringstream(t) >> A;
		ifile >> t;
		std::istringstream(t) >> B;
		ifile >> t;
		std::istringstream(t) >> C;
		ifile >> t;
		std::istringstream(t) >> D;
		ifile >> t;
		std::istringstream(t) >> E;
	}
}
//--------------------------------------------------






//-------------GETTER FUNCTIONS-------------------
int Settings::getA() { return A; }
int Settings::getB() { return B; }
int Settings::getC() { return C; }
int Settings::getD() { return D; }
int Settings::getE() { return E; }

cv::Mat Settings::getErodeElement() { return erodeElement; }
cv::Mat Settings::getDilateElement() { return dilateElement; }
//------------------------------------------------

