
#ifndef __TRACKER_H__
#define __TRACKER_H__

#include "Circle.hpp"
#include "BlobResult.h"
#include "Kalman.hpp"
#include "Uncopiable.h"
#include "timer.hpp"

class Tracker : Uncopiable<> {
    public:
        Tracker(void);
        ~Tracker(void);
        Tracker(CvSize);
        const Circle& process(IplImage* backproject) throw();
		const Circle& getNext() throw();
        bool backproject_mode_;
		Circle current;
		Circle filtered;
		Kalman kalmanFilter_;
    private:
		Timer t1;
		Timer t2;
        CvMat* state;
        CBlobResult blobs;
        CBlob *currentBlob;
		boost::mutex trackMutex_;
		bool bMeasurement_; 
        CvSize size_;
		bool gotBlob;
		bool findBlob(IplImage* image,IplImage* mask);
};

#endif
