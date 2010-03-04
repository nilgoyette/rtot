#pragma once

#include "cv.h"

class Circle
{

public:
    int radius;
    CvPoint center;
    CvRect bounds;
    bool empty;

    Circle(void)
    {
    }

    ~Circle(void)
    {
    }
};
