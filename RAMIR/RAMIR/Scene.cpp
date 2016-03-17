/*
A scene is a ROI whithin which blobs will be processed in the 
algorithm, the outer rectangle is the ROI defined by the user.
The eeline shall be drawn within the ROI.

SCENE:
________________________________
|				|				|
|				|				|
|				|eeline			|
|				|				|
|				|				|
|_______________|_______________|

*/

#include "Scene.hpp"

/**************************************************************************************************************************************
/	ARGUMENTS: entry/exit-line koordinates, region of interest
**************************************************************************************************************************************/
Scene::Scene(int arg_sx, int arg_sy, int arg_ex, int arg_ey, Mat ROI) {
	
	assert(sx == ex);					//Debug, the line must be vertical
	pauseForLineCheckDEBUG = true;		//Debug, se declaration

	//sets the eeline-koordinates
	sx = arg_sx;
	sy = arg_sy;
	ex = arg_ex;
	ey = arg_ey;

	this->ROI = ROI;
	

}
Scene::~Scene() {

}

/**************************************************************************************************************************************
/	Line-side check, checks on which side of the line an object is
/	If the object is exactly on the line, it's said to be on the right side
**************************************************************************************************************************************/
int Scene::LSCheck(Tracker t) {
	/*
	/	Tests:
	/		(1) the entry/exit-line shall be vertical
	/		(2) manual objectcheck, object on the right or left side
	/
	*/
	Blob b = t.getLastBlob();
	Point2f p = b.getCent();
	int blobx = p.x;
	int bloby = p.y;

	assert(sx == ex);						//(1) DEBUG

	if (blobx - sx < 0) {
		if (pauseForLineCheckDEBUG) {		//(2) DEBUG, 
			pauseForLineCheckDEBUG = false;	//(2) DEBUG, PLEASE CHECK SO OBJECT REALLY IS ON LEFT SIDE
		}

		return LEFTSIDE_OFLINE;				//if blob is on the left side
	} 
	else {
		if (!pauseForLineCheckDEBUG) {		//(2) DEBUG
			pauseForLineCheckDEBUG = true;	//(2) DEBUG, PLEASE CHECK SO OBJECT REALLY IS ON RIGHT SIDE
		}
		return RIGHTSIDE_OFLINE;			//if blob is on the right side
	}
	
}

int Scene::LSCheck(Blob b) {
	/*
	/	Tests:
	/		(1) the entry/exit-line shall be vertical
	/
	*/
	Point2f p = b.getCent();
	int blobx = p.x;
	int bloby = p.y;

	assert(sx == ex);						//(1) DEBUG

	if (blobx - sx < 0) {
		return LEFTSIDE_OFLINE;				//if blob is on the left side
	}
	else {
		return RIGHTSIDE_OFLINE;			//if blob is on the right side
	}

}

//Within ROI check, checks if the tracked blob is within the ROI defined by the user
bool Scene::WRCheck(Tracker t) {
	return true;
}