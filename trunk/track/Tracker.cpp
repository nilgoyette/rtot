
#include "Tracker.h"

#include <iostream>

#include "myassert.h"

Tracker::Tracker(CvSize size) : size_(size) {
	backproject_mode_ = true;	
	t1.start();
	t2.start();
}

Tracker::~Tracker(void) {}

const Circle& Tracker::process(IplImage* backproject) throw() {
	boost::lock_guard<boost::mutex> lock(trackMutex_);
 	gotBlob = findBlob(backproject, NULL);
	if (gotBlob) {
		return current;
	}
    return current;
}

const Circle& Tracker::getNext() throw() {
	boost::lock_guard<boost::mutex> lock(trackMutex_);
	filtered = kalmanFilter_.getNext((t2.elapsed() / 1000));
	return filtered;
}

bool Tracker::findBlob(IplImage* image,IplImage* mask) {
	blobs = CBlobResult(image, mask, 0);
	CBlob blobWithBiggestPerimeter;
	if (blobs.GetNumBlobs() > 0) {
		blobs.GetNthBlob(CBlobGetArea(),0,blobWithBiggestPerimeter);
		CBlob *currentBlob = blobs.GetBlob(0);
		current.init(blobWithBiggestPerimeter.GetBoundingBox());
		if (static_cast<int>(current.radius_) > 7){
			kalmanFilter_.setNext((t1.elapsed() / 1000) ,current);
			return true;
		} else {
			return false;
		}
	}
	return false;
}
