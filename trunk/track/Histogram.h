
#ifndef __HISTOGRAM_H__
#define __HISTOGRAM_H__

#include "cv.h"

enum Tracking { UNINITIALIZED = -1, INITIALIZED = 0, TRACKING = 1};

class Histogram {
	public:
		bool select_object_;
	    CvHistogram *hist_;
		IplImage* planes_[2];

		Histogram(CvSize);
		~Histogram(void);

		void createHistogram(IplImage* frame, CvRect selection,const CvArr* mask = 0);
		void getBackProject(IplImage* frame, IplImage*& backproject);
		void show();
		CvHistogram* process(IplImage* frame, CvRect selection);
	private:
		
		CvSize size_;
		
		int hdims_;
		IplImage* hsv_;
		IplImage* c_h_;
		IplImage* c_s_;
		int h_bins_;
		int s_bins_;

		int scale_;
		IplImage* hist_img_;
};

#endif
