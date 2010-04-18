#pragma once

#include <windows.h>

class TimedImage
{
public:
    DWORD timeStamp_;
    IplImage *image_;
    TimedImage(IplImage *pimage, DWORD ptimestamp):
        timeStamp_(ptimestamp),
        image_(pimage)
    {
    }

    TimedImage()
        : timeStamp_(0),
          image_(NULL)
    {
    }
};
