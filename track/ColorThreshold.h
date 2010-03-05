#if !defined(__COLORTHRESHOLD_H__)
#define __COLORTHRESHOLD_H__

#include "cv.h"
#include "Histogram.h"

class ColorThreshold
{
    IplImage* backproject_;
    int threshold_;
    IplConvKernel *se21_;
    IplConvKernel *se11_;
    bool calcHist_;
public:


    Histogram hist_;
    ColorThreshold(CvSize);
    ~ColorThreshold(void);

    void setHist();
    IplImage* process(IplImage*);
};

#endif
