#include "Histogram.h"
#include "highgui.h"

using std::min;
using std::max;
using std::abs;

Histogram::Histogram(CvSize size) 
    : hdims_(16), hsv_(cvCreateImage( size, 8, 3 )), c_h_(cvCreateImage( size, 8, 1 )),
    c_s_(cvCreateImage( size, 8, 1 )), h_bins_(15), s_bins_(15),
    scale_(10), hist_img_( cvCreateImage( cvSize( h_bins_ * scale_, s_bins_ * scale_ ), 8, 3 )),
    track_object_(0), select_object_(0), initialized_(false)
{
    size_ = size;
    int hist_size[] = { h_bins_, s_bins_ };
    float h_ranges[] = { 0, 180 };
    float s_ranges[] = { 0, 255 };
    float* ranges[] = { h_ranges, s_ranges };
    hist_ = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
}

Histogram::~Histogram(void)
{
}

void Histogram::on_mouse( int event, int x, int y, int flags, void* param ){
    if( select_object_ ){
        selection_.x = min(x,origin_.x);
        selection_.y = min(y,origin_.y);
        selection_.width = selection_.x + abs(x - origin_.x);
        selection_.height = selection_.y + abs(y - origin_.y);
        selection_.x = max( selection_.x, 0 );
        selection_.y = max( selection_.y, 0 );
        selection_.width = min( selection_.width, size_.width );
        selection_.height = min( selection_.height, size_.height );
        selection_.width -= selection_.x;
        selection_.height -= selection_.y;
    }

    switch( event )
    {
        case CV_EVENT_LBUTTONDOWN:
            origin_ = cvPoint(x,y);
            selection_ = cvRect(x,y,0,0);
            select_object_ = true;
            break;
        case CV_EVENT_LBUTTONUP:
            select_object_ = false;
            if( selection_.width > 0 && selection_.height > 0 ){
                track_object_ = true;
            }
            break;
    }
}

IplImage* Histogram::aplyRoiToImage(IplImage* frame){
    if( select_object_ && selection_.width > 0 && selection_.height > 0 ) {
        cvSetImageROI( frame, selection_ );
        cvXorS( frame, cvScalarAll(255), frame, 0 );
        cvResetImageROI( frame );
    }

    return frame;
}

void Histogram::createHistogram(IplImage* frame){
    cvCvtColor( frame, hsv_, CV_BGR2Lab );
    cvSplit( hsv_, NULL, c_h_,c_s_ , 0 );
    cvSetImageROI( c_h_, selection_ );
    cvSetImageROI( c_s_, selection_ );
    cvSetImageROI( hsv_, selection_ );
    planes_[0] = c_h_;
    planes_[1] = c_s_;
    float max_val = 0.f;
    cvSetImageROI( hsv_, selection_ );

    cvCalcHist( planes_, hist_, 0, 0 ); // Compute histogram

    cvGetMinMaxHistValue( hist_, 0, &max_val, 0, 0 );
    cvConvertScale( hist_->bins, hist_->bins, max_val ? 255. / max_val : 0., 0 );
    cvResetImageROI( c_h_ );
    cvResetImageROI( c_s_ );
    cvResetImageROI( hsv_ );
    //calculate histogram image
    
    cvZero ( hist_img_ );
    float max_value = 0;
    cvGetMinMaxHistValue( hist_, 0, &max_value, 0, 0 );
    for( int h = 0; h < h_bins_; h++ ){
        for( int s = 0; s < s_bins_; s++ ){
            float bin_val = cvQueryHistValue_2D( hist_, h, s );
            int intensity = cvRound( bin_val * 255 / max_value );
            cvRectangle( hist_img_, cvPoint( h*scale_, s*scale_ ),
                cvPoint( (h+1)*scale_ - 1, (s+1)*scale_ - 1 ),
                CV_RGB( intensity, intensity, intensity ),
                CV_FILLED );
        }
    }
    cvNamedWindow( "H-S Histogram", CV_WINDOW_AUTOSIZE) ;
    cvShowImage( "H-S Histogram", hist_img_ );
    initialized_ = true;
}

void Histogram::turnOffTracking(){
    track_object_ = false;
    initialized_ = false;
    cvDestroyWindow( "H-S Histogram" );
}

CvHistogram* Histogram::process(IplImage* frame){
    if( !initialized_ ){
        createHistogram(frame);
    }
    return hist_;
}
