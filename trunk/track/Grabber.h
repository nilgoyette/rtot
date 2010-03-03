#if !defined(__GRABBER_H__)
#define __GRABBER_H__

#include <boost/thread.hpp>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "TripleBuffering.h"
#include "dataStructures.h"

class Grabber
{
private:
	int camId_;
	CvCapture* capture;
	IplImage *frame;
    bool initialized;
	TripleBuffering& sink;

	IplImage *image;

	imageData_t imgPointers;

	IplImage *histimg;

	CvHistogram *hist;

	float hranges_arr[2];
	float* hranges;
	int hdims;

	Grabber(const Grabber&);

public:
	Grabber(int,CvSize,TripleBuffering&);
	~Grabber(void);

	void grabber(void);
    void operator()();
	

};

#endif
