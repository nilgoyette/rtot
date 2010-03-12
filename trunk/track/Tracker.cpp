
#include "Tracker.h"
#include <iostream>

Tracker::Tracker(CvSize size) : size_(size){
	mask_ = cvCreateImage(size_,8,1);
	//essayer CalcOpticalFlowPyrLK pourais etre cool aussi
	int dynam_params = 6;
    int measure_params = 3; 
    int control_params = 0;
    backproject_mode_ = true;
    kalman_ = cvCreateKalman(dynam_params, measure_params,0);
	state_ = cvCreateMat(measure_params, 1, CV_32FC1); // (phi, delta_phi)
	// F matrix data 
	// F is transition matrix.  It relates how the states interact 
	// For single input fixed velocity the new value 
	// depends on the previous value and velocity- hence 1 0 1 0 
	// on top line. New velocity is independent of previous 
	// value, and only depends on previous velocity- hence 0 1 0 1 on second row 
	const float A[] = { 
		1, 0, 0, 1, 0, 0, //x + dx 
		0, 1, 0, 0, 1, 0, //y + dy 
		0, 0, 1, 0, 0, 1, //z + dz 
		0, 0, 0, 1, 0, 0, //dx = dx
		0, 0, 0, 0, 1, 0, //dy = dy
        0, 0, 0, 0, 0, 1, //dz = dz
	};
	const float H[] = { 
		1, 0, 0, 0, 0, 0, //x + dx 
		0, 1, 0, 0, 0, 0, //y + dy 
		0, 0, 1, 0, 0, 0, //z + dz 
	};

	memcpy( kalman_->transition_matrix->data.fl, A, sizeof(A)); 
	memcpy( kalman_->measurement_matrix->data.fl, H, sizeof(H)); 

	//cvSetIdentity( kalman_->measurement_matrix, cvRealScalar(1) ); // (H) 
	cvSetIdentity( kalman_->error_cov_post, cvRealScalar(1)); 
	cvSetIdentity( kalman_->process_noise_cov, cvRealScalar(1e-20) ); // (Q) 
	cvSetIdentity( kalman_->measurement_noise_cov, cvRealScalar(1e-20) ); // (R) 
	cvZero(kalman_->state_post);
	cvZero(kalman_->state_pre);
	gotBlob = false;
}

Tracker::~Tracker(void){
	cvReleaseMat( &state_ ); 
	cvReleaseKalman( &kalman_ ); 
	cvReleaseImage(&mask_);
}

const Circle& Tracker::process(IplImage* backproject) throw() {
    blobs = CBlobResult(backproject, NULL, 0);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, 5000);
	setMask();
   // if ( findBlob(backproject,mask_) )
   //	return current;
	if ( findBlob(backproject,NULL) )
		return current;
	getPrediction(filtered);
    return filtered;
}

void Tracker::setMask(){
	int roisize  = int(current.radius_ * 2.5) ;
	int x = std::max(0,current.center_.x - roisize);
	int y = std::max(0,current.center_.y - roisize);
	x = std::min(size_.width-1,x);
	y = std::min(size_.height-1,y);
	if(x != 0 && y !=0 && roisize > 0){
		cvZero( mask_ );
		cvSetImageROI( mask_, cvRect(x,y,roisize*2,roisize*2) );
		cvXorS( mask_, cvScalarAll(255), mask_, 0 );
		cvResetImageROI( mask_ );
	}
}
bool Tracker::findBlob(IplImage* image,IplImage* mask){
	blobs = CBlobResult(image,mask, 0);
	blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER,40000);
	if (blobs.GetNumBlobs() > 0) {
		CBlob *currentBlob = blobs.GetBlob(0);
		current.init(currentBlob->GetBoundingBox());
		if (current.radius_ > 5){
			filtered.init(currentBlob->GetBoundingBox());
			updatePrediction(filtered);
			getPrediction(filtered);
			return true;
		}else {
			return false;
		}
		
	}
	return false;
}

      
void Tracker::updatePrediction(Circle &c){
	state_->data.fl[0] = (float)(c.center_.x);	//center x 
	state_->data.fl[1] = (float)(c.center_.y);	//center y 
	state_->data.fl[2] = (float)(c.radius_);	//center y 
	bMeasurement_ = true;  
}

void Tracker::getPrediction(Circle &c) {
	const CvMat* prediction = cvKalmanPredict( kalman_, 0 ); 
	c.center_.x = (int)prediction->data.fl[0]; 
	c.center_.y = (int)prediction->data.fl[1];
    c.radius_   = (int)prediction->data.fl[2];
	// If we have received the real position recently, then use that position to correct 
	// the kalman filter.  If we haven't received that position, then correct the kalman 
	// filter with the predicted position 
	if (bMeasurement_) { 
		bMeasurement_ = false;
	// My kalman filter is running much faster than my measurements
    // are coming in.  So this allows the filter to pick up from the 
	// last predicted state and continue in between measurements 
	// (this helps the tracker look smoother) 
	}  else { 
		// update with the predicted position 	
		state_->data.fl[0] = (float)c.center_.x; 
		state_->data.fl[1] = (float)c.center_.y; 
	//	state_->data.fl[2] = (float)c.radius_; 
	} 
	// adjust Kalman filter state 
	cvKalmanCorrect( kalman_, state_ ); 
}

