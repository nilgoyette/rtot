//////////////////////////////////////////////////////////////////////////
// http://code.google.com/p/bandp/source/browse/branches/src/init.cpp
// http://www.ee.surrey.ac.uk/Projects/SEP/0607/vision/Code/Onboard%20Visual%20System/kalman.cpp
//////////////////////////////////////////////////////////////////////////

#ifdef _CH_
#pragma package <opencv>
#endif


#ifndef _EiC
#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <ctype.h>
#include "BlobResult.h"
#endif
void on_trackbar( int dummy );
IplImage *image = 0, *hsv = 0, *c_h = 0;
IplImage *ab = 0;
IplImage *frame = 0;
IplImage *c_s = 0, *c_v =0 , *mask = 0, *backproject = 0, *histimg = 0;
CvHistogram *hist = 0;

char tbarname1[] = "Threshold";
char tbarname2[] = "Blob Size";
int param1;
int param2;


CvPoint center;
int radius = 0;
CvScalar mean;
CvScalar stdv;

int backproject_mode = 0;
int select_object = 0;
int track_object = 0;
int show_hist = 1;
CvPoint origin;
CvRect selection;
CvRect track_window;
CvBox2D track_box;
CvConnectedComp track_comp;
int hdims = 16;
float hranges_arr[] = {0,180};
float* hranges = hranges_arr;
int vmin = 10, vmax = 256, smin = 30;



int temporal_filter(int radius,double alpha){
	static int radius1 = 0;
	static int radius2 = 0;
	radius1 = radius2;
	radius2 = radius1*alpha + (1-alpha)*radius;
	return radius2;
}
int temporal_filter1(int radius,double alpha){
	static int radius1 = 0;
	static int radius2 = 0;
	radius1 = radius2;
	radius2 = radius1*alpha + (1-alpha)*radius;
	return radius2;
}
int temporal_filter2(int radius,double alpha){
	static int radius1 = 0;
	static int radius2 = 0;
	radius1 = radius2;
	radius2 = radius1*alpha + (1-alpha)*radius;
	return radius2;
}

void on_mouse( int event, int x, int y, int flags, void* param )
{
	if( !image )
		return;

	if( image->origin )
		y = image->height - y;

	if( select_object )
	{
		selection.x = MIN(x,origin.x);
		selection.y = MIN(y,origin.y);
		selection.width = selection.x + CV_IABS(x - origin.x);
		selection.height = selection.y + CV_IABS(y - origin.y);

		selection.x = MAX( selection.x, 0 );
		selection.y = MAX( selection.y, 0 );
		selection.width = MIN( selection.width, image->width );
		selection.height = MIN( selection.height, image->height );
		selection.width -= selection.x;
		selection.height -= selection.y;
	}

	switch( event )
	{
	case CV_EVENT_LBUTTONDOWN:
		origin = cvPoint(x,y);
		selection = cvRect(x,y,0,0);
		select_object = 1;
		break;
	case CV_EVENT_LBUTTONUP:
		select_object = 0;
		if( selection.width > 0 && selection.height > 0 )
			track_object = -1;
		break;
	}
}


CvScalar hsv2rgb( float c_h ) {
	int rgb[3], p, sector;
	static const int sector_data[][3]=
	{{0,2,1}, {1,2,0}, {1,0,2}, {2,0,1}, {2,1,0}, {0,1,2}};
	c_h *= 0.033333333333333333333333333333333f;
	sector = cvFloor(c_h);
	p = cvRound(255*(c_h - sector));
	p ^= sector & 1 ? 255 : 0;
	rgb[sector_data[sector][0]] = 255;
	rgb[sector_data[sector][1]] = 0;
	rgb[sector_data[sector][2]] = p;
	return cvScalar(rgb[2], rgb[1], rgb[0],0);
}

int maIn(int argc, char** argv) {
	param1 = 100;
	param2 = 2000;
	CvCapture* capture = 0;
	if (argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0]))) {
		capture = cvCaptureFromCAM(0);
		cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 320);
		cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 240);
	} else if (argc == 2)
		capture = cvCaptureFromAVI( argv[1] );

	if (!capture) {
		fprintf(stderr,"Could not initialize capturing...\n");
		return -1;
	}

	printf( "Hot keys: \n"
		"\tESC - quit the program\n"
		"\tc - stop the tracking\n"
		"\tb - switch to/from backprojection view\n"
		"\th - show/hide object histogram\n"
		"To initialize tracking, select the object with mouse\n" );

	//cvNamedWindow( "Histogram", 1 );
	cvNamedWindow( "CamShiftDemo", 1 );
	cvSetMouseCallback( "CamShiftDemo", on_mouse, 0 );
	cvCreateTrackbar( tbarname1, "CamShiftDemo", &param1, 255, on_trackbar );

	CvKalman* kalman = cvCreateKalman( 2, 2, 0 );
	CvMat* state = cvCreateMat( 2, 1, CV_32FC1 ); /* (phi, delta_phi) */
	
	cvSetIdentity(kalman->measurement_matrix, cvRealScalar(1)); //identity
	cvSetIdentity(kalman->error_cov_post, cvRealScalar(1)); //identity
	cvSetIdentity(kalman->process_noise_cov, cvRealScalar(0.4));
	cvSetIdentity(kalman->measurement_noise_cov, cvRealScalar(3));
	
            frame = cvQueryFrame( capture ); //DO NOT RELEASE THIS IMAGE
			image = cvCreateImage( cvGetSize(frame), 8, 3 );

			image->origin = frame->origin;
			hsv = cvCreateImage( cvGetSize(frame), 8, 3 );
			c_h = cvCreateImage( cvGetSize(frame), 8, 1 );
            c_s = cvCreateImage( cvGetSize(frame), 8, 1 );
			c_v = cvCreateImage( cvGetSize(frame), 8, 1 );
			
			backproject = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 1 );

			hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );
			histimg = cvCreateImage( cvSize(320,200), 8, 3 );


	for(;;) {
		int i, bin_w, c;
		frame = cvQueryFrame( capture ); //DO NOT RELEASE THIS IMAGE
		if (!frame)
			break;

		if (!image) {
			/* allocate all the buffers */

		}
		
		//cvSmooth(frame, frame, CV_BLUR, 15, 15, 0, 0);
		cvCvtColor( frame, hsv, CV_BGR2Lab );

		if (track_object) {
			int _vmin = vmin, _vmax = vmax;
			cvSplit( hsv, NULL, c_h,c_s , 0 );
			
            IplImage* planes[] = { c_h, c_s};
			if (track_object < 0) {
				float max_val = 0.f;				
				cvSetImageROI( c_h, selection );
				cvSetImageROI( c_s, selection );
			    cvSetImageROI( hsv, selection );
				cvAvgSdv( hsv, &mean, &stdv, NULL );
				int h_bins = 15, s_bins = 15; 
				{
					int hist_size[] = { h_bins, s_bins };
					float h_ranges[] = { 0, 180 };
					float s_ranges[] = { 0, 255 };
					float* ranges[] = { h_ranges, s_ranges };
					hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
				}
				
                cvCalcHist( planes, hist, 0, 0 ); // Compute histogram
				
				cvGetMinMaxHistValue( hist, 0, &max_val, 0, 0 );
				cvConvertScale( hist->bins, hist->bins, max_val ? 255. / max_val : 0., 0 );
				cvResetImageROI( c_h );
				cvResetImageROI( c_s );
				cvResetImageROI( hsv );
				track_object = 1;
				//calculate histogram image
				int scale = 10;
				IplImage* hist_img = cvCreateImage( cvSize( h_bins * scale, s_bins * scale ), 8, 3 );
                cvZero ( hist_img );	
			    float max_value = 0;
				cvGetMinMaxHistValue( hist, 0, &max_value, 0, 0 );
				for( int h = 0; h < h_bins; h++ ){
					for( int s = 0; s < s_bins; s++ ){
						
						float bin_val = cvQueryHistValue_2D( hist, h, s );
						int intensity = cvRound( bin_val * 255 / max_value );
						cvRectangle( hist_img, cvPoint( h*scale, s*scale ),
							cvPoint( (h+1)*scale - 1, (s+1)*scale - 1 ),
							CV_RGB( intensity, intensity, intensity ),
							CV_FILLED );
					}
				}
				cvNamedWindow( "H-S Histogram", 1) ;
				cvShowImage( "H-S Histogram", hist_img );
			}
            //calcule image backproject using histogram
			cvCalcBackProject( planes, backproject, hist );
           
			IplConvKernel *se21 = cvCreateStructuringElementEx((10*2)+1, (10*2)+1, 10, 10, CV_SHAPE_RECT, NULL);
			IplConvKernel *se11 = cvCreateStructuringElementEx((3*2)+1, (3*2)+1, 3,  3,  CV_SHAPE_RECT, NULL);			
			cvMorphologyEx(backproject, backproject, NULL,se21,CV_MOP_CLOSE); // See completed example for cvClose definition	
			cvMorphologyEx(backproject, backproject, NULL, se11,CV_MOP_OPEN );  // See completed example for cvOpen  definition
			cvReleaseStructuringElement(&se21);
			cvReleaseStructuringElement(&se11);
			//cvSmooth(backproject, backproject, CV_GAUSSIAN, 15, 15, 0, 0);

			cvSmooth(backproject, backproject, CV_GAUSSIAN, 5,5, 0, 0);
			cvThreshold(backproject,backproject,param1,255,CV_THRESH_BINARY | CV_THRESH_OTSU );	
			
			
			if (backproject_mode){
				
				//display in black and white
				//cvMerge( backproject, backproject, backproject, NULL, frame );
				CBlobResult blobs;			
				CBlob *currentBlob;
				blobs = CBlobResult( backproject, NULL, 0 );
				blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, 5000 );
				// display filtered blobs
				if ( blobs.GetNumBlobs() > 0){
					currentBlob = blobs.GetBlob(0);
					CvRect boundingBox = currentBlob->GetBoundingBox();
					//CBlob theBlob;
                    //blobs.GetNthBlob(CBlobGetArea(),blobs.GetNumBlobs(),theBlob);
					//CvRect boundingBox = theBlob.GetBoundingBox();
					int height = boundingBox.height;
					int width = boundingBox.width;
					int max =  height > width ? height : width; 
					int x = boundingBox.x + (width/2);
					int y = boundingBox.y + (height/2);
					//x = temporal_filter1(x,0.30);
					//y = temporal_filter2(y,0.30);   			
                    /*
					const CvMat* prediction = cvKalmanPredict(kalman);
					center = cvPoint((int)cvmGet(prediction, 0, 0),
						(int)cvmGet(prediction, 1, 0)
						);

					cvmSet(state, 0, 0, x);
					cvmSet(state, 1, 0, y);
					cvKalmanCorrect(kalman, state);
                    */
                    center = cvPoint(x,y);
					radius = temporal_filter((max/2),0.75);
		     		//cvCircle(frame,cvPoint(x,y),radius,CV_RGB(255,0,0),3);
					//cvCircle(frame,cvPoint(x,y), 3,CV_RGB(255,0,0), -1, CV_AA, 0);
					cvCircle(frame,center,radius,CV_RGB(0,0,255),3);
					cvCircle(frame,center, 3,CV_RGB(0,0,255), -1, CV_AA, 0);
					CvFont myFont;
					cvInitFont(&myFont,CV_FONT_HERSHEY_PLAIN ,2.0f,1.0f);
					cvPutText( frame, "30 fps", cvPoint(50,30), &myFont , cvScalar(128) );
				}
				
				
			}
		}
        
		if( select_object && selection.width > 0 && selection.height > 0 )
		{
			cvSetImageROI( frame, selection );
			cvXorS( frame, cvScalarAll(255), frame, 0 );
			cvResetImageROI( frame );
		}

		  cvShowImage( "CamShiftDemo", frame );

		c = cvWaitKey(10);
		if( (char) c == 27 )
			break;
		switch( (char) c )
		{
		case 'b':
			backproject_mode ^= 1;
			break;
		case 'c':
			track_object = 0;
			cvZero( histimg );
			break;
		case 'h':
			show_hist ^= 1;
			if( !show_hist )
				cvDestroyWindow( "Histogram" );
			else
				cvNamedWindow( "Histogram", 1 );
			break;
		default:
			;
		}
	}

	cvReleaseCapture( &capture );
	cvDestroyWindow("CamShiftDemo");

	return 0;
}

// threshold trackbar callback
void on_trackbar( int dummy )
{
	/*
	// threshold input image
	cvThreshold( backproject, backproject, param1, 255, CV_THRESH_BINARY );

	// get blobs and filter them using its area
	CBlobResult blobs;
	int i;
	CBlob *currentBlob;

	// find blobs in image

	blobs = CBlobResult( originalThr, NULL, 255 );
	blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, param2 );

	// display filtered blobs
	cvMerge( originalThr, originalThr, originalThr, NULL, displayedImage );

	for (i = 0; i < blobs.GetNumBlobs(); i++ )
	{
		currentBlob = blobs.GetBlob(i);
		currentBlob->FillBlob( displayedImage, CV_RGB(255,0,0));
	} 
	*/
}



#ifdef _EiC
main(1,"camshiftdemo.c");
#endif
