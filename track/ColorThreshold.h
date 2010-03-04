#pragma once
#include "cv.h"

class ColorThreshold
{
    IplImage* backproject_;
    int threshold_;

public:
    ColorThreshold(CvSize);
    ~ColorThreshold(void);

    IplImage* process(IplImage*);
};
