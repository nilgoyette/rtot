
#include "Histogram.h"

#include "highgui.h"

using std::min;
using std::max;
using std::abs;

Histogram::Histogram(CvSize size) 
    : hdims_(16),
	  hsv_(cvCreateImage(size, 8, 3)),
	  c_h_(cvCreateImage(size, 8, 1)),
      c_s_(cvCreateImage(size, 8, 1)),
	  h_bins_(15),
	  s_bins_(15),
      scale_(10),
	  hist_img_(cvCreateImage(cvSize(h_bins_ * scale_, s_bins_ * scale_ ), 8, 3 )),
      select_object_(0) {
    size_ = size;
    int hist_size[] = {h_bins_, s_bins_};
    float h_ranges[] = {0, 180};
    float s_ranges[] = {0, 255};
    float* ranges[] = {h_ranges, s_ranges};
    hist_ = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
}

Histogram::~Histogram(void) {
    cvReleaseImage(&hsv_);
    cvReleaseImage(&c_h_);
    cvReleaseImage(&c_s_);
    cvReleaseImage(&hist_img_);
    cvReleaseHist(&hist_);
}

void Histogram::createHistogram(IplImage* frame,CvRect selection) {
    cvCvtColor(frame, hsv_, CV_BGR2Lab);
    cvSplit(hsv_, NULL, c_h_,c_s_ , 0);
    cvSetImageROI(c_h_, selection);
    cvSetImageROI(c_s_, selection);
    cvSetImageROI(hsv_, selection);
	cvSetImageROI(hsv_, selection);

    planes_[0] = c_h_;
    planes_[1] = c_s_;
    float max_val = 0.f;
    
    cvCalcHist(planes_, hist_, 0, 0); // Compute histogram
    cvNormalizeHist(hist_, 255);
    cvGetMinMaxHistValue(hist_, 0, &max_val, 0, 0);
    cvConvertScale(hist_->bins, hist_->bins, max_val ? 255.0 / max_val : 0.0, 0);
    cvResetImageROI(c_h_);
    cvResetImageROI(c_s_);
    cvResetImageROI(hsv_);

    // Calculate histogram image
    cvZero(hist_img_);
    float max_value = 0;
    cvGetMinMaxHistValue(hist_, 0, &max_value, 0, 0);
    for (int h = 0; h < h_bins_; h++) {
        for (int s = 0; s < s_bins_; s++) {
            float bin_val = cvQueryHistValue_2D(hist_, h, s);
            int intensity = cvRound(bin_val * 255 / max_value);
            cvRectangle(hist_img_, cvPoint(h * scale_, s * scale_),
                cvPoint((h + 1) * scale_ - 1, (s + 1) * scale_ - 1),
                CV_RGB(intensity, intensity, intensity), CV_FILLED);
        }
    }
	
    //cvNamedWindow( "H-S Histogram", CV_WINDOW_AUTOSIZE) ;
    //cvShowImage( "H-S Histogram", hist_img_ );
}

void Histogram::getBackProject(IplImage* frame, IplImage*& backproject) {
	cvCvtColor(frame, hsv_, CV_BGR2Lab);
	cvSplit(hsv_, NULL, c_h_, c_s_, 0);
    IplImage* planes[] = {c_h_, c_s_};
	cvCalcBackProject(planes, backproject, hist_);
}

CvHistogram* Histogram::process(IplImage* frame, CvRect selection){
   /* if( !initialized_ ){
        createHistogram(frame,selection);
    }
	*/
    return NULL;
}
