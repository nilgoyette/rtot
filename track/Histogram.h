#if !defined(__HISTOGRAM_H__)
#define __HISTOGRAM_H__

#include "cv.h"

class Histogram
{
    CvSize size_;
    CvPoint origin_;
    int hdims_;
    IplImage* hsv_;
    IplImage* c_h_;
    IplImage* c_s_;
    IplImage* histimg_;
    int h_bins_;
    int s_bins_;
    CvHistogram *hist_;
    int scale_;
    IplImage* hist_img_;
public:
    int track_object_;
    int select_object_;
    CvRect selection_;
    IplImage* planes_[2];

    Histogram(CvSize);
    ~Histogram(void);

    void createHistogram(IplImage* frame);
    CvHistogram* process(IplImage* frame);
    void on_mouse( int event, int x, int y, int flags, void* param );
};

#endif
