#if !defined(__GRABBER_H__)
#define __GRABBER_H__

#include <boost/thread.hpp>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "dataStructures.h"

class Grabber
{
private:
	int camId_;
	int width_;
	int height_;

	boost::mutex mut;

	CvCapture* capture;
	
	IplImage *frame;
	IplImage *image;

	imageData_t imgPointers;

	IplImage *histimg;

	CvHistogram *hist;

	float hranges_arr[2];
	float* hranges;
	int hdims;

public:
	Grabber(int camId, int width, int hieght);
	~Grabber(void);

	void grabber(void);

	void threadStart(void);

};

#endif
