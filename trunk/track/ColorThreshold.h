
#ifndef __COLORTHRESHOLD_H__
#define __COLORTHRESHOLD_H__
// lightness range in which color are acurate
#define COLOR_VMAX 256
#define COLOR_VMIN 15  
// minimum saturation 
#define COLOR_SMIN 20
#include "cv.h"
#include "Histogram.h"
#include "Tracker.h"
#include "TripleBuffering.h"
class ColorThreshold {
	public:
		void on_mouse(int event, int x, int y, int flags, void* param);
		void turnOffTracking();
		bool track_object_;    
		bool calcule_hist_;
		ColorThreshold(CvSize, TripleBuffering&, Tracker&);
		~ColorThreshold(void);
		void operator()() throw();
		IplImage* process(IplImage*);
		CvRect selection_;
		void exit();

	private:
		TripleBuffering& source_; // Concurrent image queue
		Tracker& track_;
		IplImage* backproject_;
		IplImage* hsv_;
		IplImage* mask_;
		int threshold_;
		IplConvKernel *se21_;
		IplConvKernel *seHistogram1_;
		IplConvKernel *seHistogram2_;
		IplConvKernel *se11_;
		bool calcHist_;
		CvPoint origin_;
		CvSize size_;
		bool initialized_;
		Histogram hist_;
		volatile bool exit_;
};

#endif
