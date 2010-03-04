#include "ColorThreshold.h"

ColorThreshold::ColorThreshold(CvSize size) : threshold_(100), backproject_(cvCreateImage(size,8,1))
{
}

ColorThreshold::~ColorThreshold(void)
{
}

IplImage* ColorThreshold::process(IplImage* img){

    //float max_val = 0.f;
    //cvSetImageROI( hsv, selection );
    //cvAvgSdv( hsv, &mean, &stdv, NULL );
    //int h_bins = 15, s_bins = 15; 
    //{
    //    int hist_size[] = { h_bins, s_bins };
    //    float h_ranges[] = { 0, 180 };
    //    float s_ranges[] = { 0, 255 };
    //    float* ranges[] = { h_ranges, s_ranges };
    //    hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
    //}

    //cvCalcHist( planes, hist, 0, 0 ); // Compute histogram

    //cvGetMinMaxHistValue( hist, 0, &max_val, 0, 0 );
    //cvConvertScale( hist->bins, hist->bins, max_val ? 255. / max_val : 0., 0 );
    //cvResetImageROI( hsv );
    //track_object = 1;
    ////calculate histogram image
    //int scale = 10;
    //IplImage* hist_img = cvCreateImage( cvSize( h_bins * scale, s_bins * scale ), 8, 3 );
    //cvZero ( hist_img );	
    //float max_value = 0;
    //cvGetMinMaxHistValue( hist, 0, &max_value, 0, 0 );
    //for( int h = 0; h < h_bins; h++ ){
    //    for( int s = 0; s < s_bins; s++ ){
    //        float bin_val = cvQueryHistValue_2D( hist, h, s );
    //        int intensity = cvRound( bin_val * 255 / max_value );
    //        cvRectangle( hist_img, cvPoint( h*scale, s*scale ),
    //            cvPoint( (h+1)*scale - 1, (s+1)*scale - 1 ),
    //            CV_RGB( intensity, intensity, intensity ),
    //            CV_FILLED );
    //    }
    //}
    //cvNamedWindow( "H-S Histogram", 1) ;
    //cvShowImage( "H-S Histogram", hist_img );

    ////calcule image backproject using histogram
    //cvCalcBackProject( planes, backproject, hist );

    //IplConvKernel *se21 = cvCreateStructuringElementEx((10*2)+1, (10*2)+1, 10, 10, CV_SHAPE_RECT, NULL);
    //IplConvKernel *se11 = cvCreateStructuringElementEx((3*2)+1, (3*2)+1, 3,  3,  CV_SHAPE_RECT, NULL);
    //cvMorphologyEx(backproject_, backproject_, NULL,se21,CV_MOP_CLOSE); // See completed example for cvClose definition	
    //cvMorphologyEx(backproject_, backproject_, NULL, se11,CV_MOP_OPEN );  // See completed example for cvOpen  definition
    //cvReleaseStructuringElement(&se21);
    //cvReleaseStructuringElement(&se11);
    //cvSmooth(backproject_, backproject_, CV_GAUSSIAN, 5,5, 0, 0);
    
    
    
    cvCvtColor(img,backproject_,CV_BGR2GRAY);


    cvThreshold(backproject_,backproject_,threshold_,255,CV_THRESH_BINARY | CV_THRESH_OTSU );

    return backproject_;
}