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

Scene::Scene(int tsx, int tsy, int tex, int tey, Mat ROI) {
	
	assert(sx == ex);		//the line must be vertical
	
	//sets the eeline-koordinates
	sx = tsx;
	sy = tsy;
	ex = tex;
	ey = tey;

	this->ROI = ROI;

}
Scene::~Scene() {

}


//Line-side check, checks on which side of the line an object is
int LSCheck(Tracker t) {
	Blob b = t.getLastBlob();
	Point p = b.getCentroid();
	int blobx = p.x;
	int bloby = p.y;

	assert(sx == ex);

	if (blobx - sx < 0) {
		return LEFTSIDE_OFLINE;				//if blob is on the left side
	} 
	else {
		return RIGHTSIDE_OFLINE;			//if blob is on the right side
	}
	
}

//Within ROI check, checks if the tracked blob is within the ROI defined by the user
bool WRCheck(Tracker t) {
	return true;
}