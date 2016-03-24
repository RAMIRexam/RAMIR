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
Scene::Scene(int arg_sx, int arg_sy, int arg_ex, int arg_ey, Mat arg_ROI, Rect arg_rect) {

	assert(sx == ex);					//Debug, the line must be vertical
	pauseForLineCheckDEBUG = true;		//Debug, se declaration

										//sets the eeline-koordinates
	sx = arg_sx;
	sy = arg_sy;
	ex = arg_ex;
	ey = arg_ey;

	ROI = arg_ROI;
	rect = arg_rect;


}
Scene::Scene() {}
Scene::~Scene() {

}

Point2f Scene::getStartPos() {
	return Point2f(rect.x, rect.y);

}

/**************************************************************************************************************************************
/	Line-side check, checks on which side of the line an object is
/	If the object is exactly on the line, it's said to be on the right side
**************************************************************************************************************************************/
int Scene::LSCheck(Blob b) {
	/*
	/	Tests:
	/		(1) the entry/exit-line shall be vertical
	/		(2) manual objectcheck, object on the right or left side
	/
	*/
	Point2f p = b.getCent();
	int blobx = p.x;
	int bloby = p.y;

	assert(sx == ex);						//(1) DEBUG

	if (blobx - ROI.cols/2 < 0) {
		if (pauseForLineCheckDEBUG) {		//(2) DEBUG, 
			pauseForLineCheckDEBUG = false;	//(2) DEBUG, PLEASE CHECK SO OBJECT REALLY IS ON LEFT SIDE (add breakpoint)
		}

		return LEFTSIDE_OFLINE;				//if blob is on the left side
	} 
	else {
		if (!pauseForLineCheckDEBUG) {		//(2) DEBUG
			pauseForLineCheckDEBUG = true;	//(2) DEBUG, PLEASE CHECK SO OBJECT REALLY IS ON RIGHT SIDE (add breakpoint)
		}
		return RIGHTSIDE_OFLINE;			//if blob is on the right side
	}
	
}


//Within ROI check, checks if the tracked blob is within the ROI defined by the user
//bool Scene::WRCheck(Tracker* t) {
//	return true;
//}