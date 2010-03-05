
#ifndef __TRACKER_H__
#define __TRACKER_H__

#include "Circle.hpp"
#include "BlobResult.h"
#include "Uncopiable.h"

class Tracker : Uncopiable<> {
    public:
        Tracker(void);
        //~Tracker(void);

        const Circle& process(IplImage* backproject) throw();
        bool backproject_mode_;
    private:
        CvKalman* kalman;
        Circle current;
        CvMat* state;
        CBlobResult blobs;
        CBlob *currentBlob;
};

#endif
