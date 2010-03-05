#if !defined(__HISTOGRAM_H__)
#define __HISTOGRAM_H__

#include "cv.h"

enum Tracking { UNINITIALIZED = -1, INITIALIZED = 0, TRACKING = 1};

class Histogram
{
    CvSize size_;
    CvPoint origin_;
    int hdims_;
    IplImage* hsv_;
    IplImage* c_h_;
    IplImage* c_s_;
    int h_bins_;
    int s_bins_;
    CvHistogram *hist_;
    int scale_;
    IplImage* hist_img_;
public:
    bool select_object_;
    bool initialized_;
    bool track_object_;

    CvRect selection_;
    IplImage* planes_[2];

    Histogram(CvSize);
    ~Histogram(void);

    void turnOffTracking();
    IplImage* aplyRoiToImage(IplImage* frame);
    void createHistogram(IplImage* frame);
    CvHistogram* process(IplImage* frame);
    void on_mouse( int event, int x, int y, int flags, void* param );
};

#endif
