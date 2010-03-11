
#ifndef __TRACKER_H__
#define __TRACKER_H__

#include "Circle.hpp"
#include "BlobResult.h"
#include "Uncopiable.h"

class Tracker : Uncopiable<> {
    public:
        Tracker(void);
        //~Tracker(void);
        Tracker(CvSize);
        ~Tracker(void);

        const Circle& process(IplImage* backproject) throw();
        bool backproject_mode_;
		Circle current;
    private:
        CvKalman* kalman;
        CvMat* state;
        CBlobResult blobs;
        CBlob *currentBlob;
		IplImage* mask_;
        CvKalman* kalman_;
	    CvRandState rng_;
		CvMat* state_;
		bool bMeasurement_; 
        CvSize size_;
		bool gotBlob;

		void updatePrediction(Circle &c);
		void getPrediction(Circle &c);
		void setMask();
		bool findBlob(IplImage* image,IplImage* mask);

		
       
};

#endif
