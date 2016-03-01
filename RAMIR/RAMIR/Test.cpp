#include "Test.hpp"


static void trackerTest(vector<Tracker> trackers) {
	for (Tracker t : trackers) {
		assert(t.processed == true);
	}
}


