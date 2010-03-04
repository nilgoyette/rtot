#include "Circle.hpp"
#include "BlobResult.h"

class Tracker
{
    CvKalman* kalman;
    Circle current;
    CvMat* state;
    CBlobResult blobs;
    CBlob *currentBlob;
public:
    Tracker(void);
    ~Tracker(void);

    const Circle& process(IplImage* backproject);
};
