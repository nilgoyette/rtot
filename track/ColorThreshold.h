#if !defined(__COLORTHRESHOLD_H__)
#define __COLORTHRESHOLD_H__

#include "cv.h"
#include "Histogram.h"

class ColorThreshold
{
public:
    void on_mouse( int event, int x, int y, int flags, void* param );
	void turnOffTracking();
	bool track_object_;    
	bool calcule_hist_;
    ColorThreshold(CvSize);
    ~ColorThreshold(void);
    IplImage* process(IplImage*);
	int th;
private:
	IplImage* backproject_;
	int threshold_;
	IplConvKernel *se21_;
	IplConvKernel *se11_;
	bool calcHist_;
	CvRect selection_;
	CvPoint origin_;
	CvSize size_;
	bool initialized_;
	bool select_object_;
	Histogram hist_;

};

#endif
