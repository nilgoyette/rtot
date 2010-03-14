#if !defined(__COLORTHRESHOLD_H__)
#define __COLORTHRESHOLD_H__

#include "cv.h"
#include "Histogram.h"
#include "Tracker.h"
#include "TripleBuffering.h"
class ColorThreshold
{
public:
    void on_mouse( int event, int x, int y, int flags, void* param );
	void turnOffTracking();
	bool track_object_;    
	bool calcule_hist_;
    ColorThreshold(CvSize,TripleBuffering&,Tracker&);
    ~ColorThreshold(void);
	void operator()() throw();
    IplImage* process(IplImage*);
	CvRect selection_;
	bool select_object_;
private:
	TripleBuffering& source_; // Concurrent image queue
	Tracker& track_;
	IplImage* backproject_;
	int threshold_;
	IplConvKernel *se21_;
	IplConvKernel *se11_;
	bool calcHist_;
	CvPoint origin_;
	CvSize size_;
	bool initialized_;
	Histogram hist_;

};

#endif