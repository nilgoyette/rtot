#ifdef _CH_
#pragma package <opencv>
#endif

#define CV_NO_BACKWARD_COMPATIBILITY

#ifndef _EiC
#include "cv.h"
#include <limits>
#include "highgui.h"
#include <stdio.h>
#include <ctype.h>
#endif
using namespace std;

int main()
{
	//create camera object by using device 0
	CvCapture* capture = cvCaptureFromCAM(0);
    cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,320);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,320);

	//create image object to be used
	IplImage* img = 0;
    img=cvRetrieveFrame(capture); 
	// capture a frame from the camera to see if it is working
	if(!cvGrabFrame(capture))
	{exit(0);}
	cvNamedWindow( "circles", 1 );
    IplImage* gray = cvCreateImage(cvGetSize(img), 8, 1);
    CvMemStorage* storage = cvCreateMemStorage(0);
	IplImage* lab = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
	int lval;
	int aval;
	int bval;
	int radius;
    cvCreateTrackbar("l","circles",&lval,255,NULL);
	cvCreateTrackbar("a","circles",&aval,255,NULL);
	cvCreateTrackbar("b","circles",&bval,255,NULL);
    cvCreateTrackbar("r","circles",&radius,255,NULL);
	while (true){
	//capture the first image from the camera
	img=cvRetrieveFrame(capture); 
	
	cvCvtColor(img,lab,CV_RGB2Lab);
	CvScalar s;
	double rmax = pow(radius*1.0,2);
	for (int i = 0; i< lab->height; i++){
		for (int j = 0; j< lab->width; j++){
			s=cvGet2D(lab,i,j); // get the (i,j) pixel value	
			double ldiff = lval - s.val[0];
			double adiff = aval - s.val[1];
			double bdiff = bval - s.val[2];
			double sum = pow(ldiff,2) + pow(adiff,2)+ pow(bdiff,2);
			if (sum < rmax){
			   s.val[0] = 0;
               cvSet2D(lab,i,j,s);
			}


			
		}
	}
	
	cvCvtColor(lab,img,CV_Lab2RGB);
	cvShowImage( "circles", img );
	

	//wait for the user to press anything
	if ( cvWaitKey(1) == 27)
	break;
	}

	//release camera, images, and windows
	cvReleaseCapture(&capture);
	cvReleaseImage(&gray);
	cvDestroyWindow("circles");
	cvDestroyWindow("gray");
	return 0;
}
