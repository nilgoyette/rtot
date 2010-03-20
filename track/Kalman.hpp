// Implement a Kalman filter that allow to smooth and predict 
// a 3d position by estimating velocity.
// http://bilgin.esme.org/BitsBytes/KalmanFilterforDummies/tabid/196/Default.aspx
// http://www.innovatia.com/software/papers/kalman.htm
// http://www.cs.unc.edu/~welch/kalman/maybeck.html

#ifndef __KALMAN_H__
#define __KALMAN_H__

#include <iostream>

#include <boost/thread.hpp>

#include "cv.h"

#include "Circle.hpp"
#include "myassert.h"

#define Mf(a, i, j) CV_MAT_ELEM( *(a), float, i, j )

class Kalman {
public:
	Kalman(void) : totalerr(0.0) {
		firstFrame = true;
		const int control_params = 0;
		const int measure_params = 6;
		const int dynam_params = 6;
			
		const double processNoise = 1e-6f;
        const double measurementVariance = 1e-6f;
		const double measurementZVariace = 1e-5f;
	    const double measurementVelocityVariance = 1e-5f;
		const double measurementZVelocityVariance = 1e-3f;
    

		state_ = cvCreateMat(measure_params, 1, CV_32FC1);
		predictedState_ = cvCreateMat(measure_params, 1, CV_32FC1);
		// Setup Kalman tracker with 6 model variable2, 6 measurement variables, and no control variables
		kalman_ = cvCreateKalman( dynam_params, measure_params, control_params );



		// Transition matrix F describes model parameters at and k and k+1
		// the new value depends on the previous value and velocity
		// New velocity is independent of previous value
		// and only depends on previous velocity.
		CvMat *_F = kalman_->transition_matrix;
		Mf(_F,0,0) = 1.0f; Mf(_F,0,1) = 0.0f; Mf(_F,0,2) = 0.0f; Mf(_F,0,3) = 0.067f; Mf(_F,0,4) = 0.0f; Mf(_F,0,5) = 0.0f;
		Mf(_F,1,0) = 0.0f; Mf(_F,1,1) = 1.0f; Mf(_F,1,2) = 0.0f; Mf(_F,1,3) = 0.0f; Mf(_F,1,4) = 0.067f; Mf(_F,1,5) = 0.0f;
		Mf(_F,2,0) = 0.0f; Mf(_F,2,1) = 0.0f; Mf(_F,2,2) = 1.0f; Mf(_F,2,3) = 0.0f; Mf(_F,2,4) = 0.0f; Mf(_F,2,5) = 0.067f;
		Mf(_F,3,0) = 0.0f; Mf(_F,3,1) = 0.0f; Mf(_F,3,2) = 0.0f; Mf(_F,3,3) = 1.0f; Mf(_F,3,4) = 0.0f; Mf(_F,3,5) = 0.0f;
		Mf(_F,4,0) = 0.0f; Mf(_F,4,1) = 0.0f; Mf(_F,4,2) = 0.0f; Mf(_F,4,3) = 0.0f; Mf(_F,4,4) = 1.0f; Mf(_F,4,5) = 0.0f;
		Mf(_F,5,0) = 0.0f; Mf(_F,5,1) = 0.0f; Mf(_F,5,2) = 0.0f; Mf(_F,5,3) = 0.0f; Mf(_F,5,4) = 0.0f; Mf(_F,5,5) = 1.0f;
        
		// Maps measurement coordinates into process model coordinates.
		cvSetIdentity( kalman_->measurement_matrix ); 

		// This must be necessary to keep the first feedback loop result sane.
		cvSetIdentity( kalman_->error_cov_post, cvRealScalar(1));

		// Process noise is uniform across XYW
		// it would be perfect dynamics except for the unknown control forces
		// Human noise of the hand location we try to model is still quite stable compared to measurement noise.
		cvSetIdentity( kalman_->process_noise_cov, cvRealScalar( processNoise ) );

		// Measurement noise for XY
		CvMat *MN = kalman_->measurement_noise_cov;
		cvSetIdentity( MN, cvRealScalar( 1.0 ) );
		Mf(MN,0,0) = (float)measurementVariance;
		Mf(MN,1,1) = (float)measurementVariance;
		Mf(MN,2,2) = (float)measurementZVariace;
		Mf(MN,3,3) = (float)measurementVelocityVariance;
		Mf(MN,4,4) = (float)measurementVelocityVariance;
		Mf(MN,5,5) = (float)measurementZVelocityVariance;

		cvZero(kalman_->state_post);
		cvZero(kalman_->state_pre);
		gotBlob = false;
	}

	~Kalman(void){
		if ( kalman_ != NULL ) {
			cvReleaseMat( &state_ );
			cvReleaseMat( &predictedState_ );
			
			cvReleaseKalman( &kalman_ );
			kalman_ = NULL;
		}
	}


    void setNext(double dt,Circle &c){
		if(firstFrame){
			setFirst(c);
			firstFrame = false;
			return;
		}

		// Update time change in model.
		// Average position over the last 10 (or so) frames 
		previousFrame = currentFrame;
		double pctA = 0.4;
		double pctB = 1.0-pctA;
		currentFrame = Circle(
			float(c.x_*pctA + previousFrame.x_*pctB),
			float(c.y_*pctA + previousFrame.y_*pctB),
			float(c.radius_*pctA + previousFrame.radius_*pctB)
			);
	    
		Mf(state_,0,0) = c.x_;
		Mf(state_,1,0) = c.y_;
		Mf(state_,2,0) = c.radius_;
		Mf(state_,3,0) = float((currentFrame.x_-previousFrame.x_) / dt);
		Mf(state_,4,0) = float((currentFrame.y_-previousFrame.y_) / dt);
		Mf(state_,5,0) = float((currentFrame.radius_-previousFrame.radius_) / dt) ;
		bMeasurement_ = true;  
		CvMat *_F = kalman_->transition_matrix;
		Mf(_F,0,3) = (float)dt;
		Mf(_F,1,4) = (float)dt;
		Mf(_F,2,5) = (float)dt;        
		cvKalmanCorrect( kalman_, state_ );
	}
	Circle getNext(double dt){
		CvMat *_F = kalman_->transition_matrix;
		Mf(_F,0,3) = (float)dt;
		Mf(_F,1,4) = (float)dt;
		Mf(_F,2,5) = (float)dt;
		// Actual Kalman steps
		const CvMat* prediction = cvKalmanPredict( kalman_, NULL );
		Mf(predictedState_,0,0) = Mf(kalman_->state_pre,0,0);
		Mf(predictedState_,1,0) = Mf(kalman_->state_pre,1,0);
		Mf(predictedState_,2,0) = Mf(kalman_->state_pre,2,0);
		Mf(predictedState_,3,0) = Mf(kalman_->state_pre,3,0);
		Mf(predictedState_,4,0) = Mf(kalman_->state_pre,4,0);
		Mf(predictedState_,5,0) = Mf(kalman_->state_pre,5,0);
        // My kalman filter is running faster than my measurements
		// So this allows the filter to pick up from the 
	    // last predicted state and continue in between measurements 
		// this helps the tracker look smoother 
		if (bMeasurement_) {
			// If we have received the real position recently, 
			// then use that position to correct the kalman filter.
			bMeasurement_ = false;
		}  else { 
			// If we haven't received that position, then correct  
			// with the predicted position 
			Mf(state_,0,0) = Mf(predictedState_,0,0);
			Mf(state_,1,0) = Mf(predictedState_,1,0);
			Mf(state_,2,0) = Mf(predictedState_,2,0);
			Mf(state_,3,0) = Mf(predictedState_,3,0);
			Mf(state_,4,0) = Mf(predictedState_,4,0);
			Mf(state_,5,0) = Mf(predictedState_,5,0);
			cvKalmanCorrect( kalman_, state_ );
		} 
		return Circle(prediction->data.fl[0],
			          prediction->data.fl[1],
					  prediction->data.fl[2]);
	}

	private:
		void setFirst(Circle &c) {
			totalerr = 0.0;
			Mf(kalman_->state_post,0,0) = c.x_;
			Mf(kalman_->state_post,1,0) = c.y_;
			Mf(kalman_->state_post,2,0) = c.radius_;
			Mf(kalman_->state_post,3,0) = c.x_;
			Mf(kalman_->state_post,4,0) = c.y_;
			Mf(kalman_->state_post,5,0) = c.radius_;
			currentFrame = c;
			previousFrame = c;
		}

		double calculateDistance() {
			return sqrt(pow(Mf(predictedState_,0,0) - Mf(state_,0,0),2) +
				pow(Mf(predictedState_,1,0) - Mf(state_,1,0),2) +
				pow(Mf(predictedState_,2,0) - Mf(state_,2,0),2));
		}

		bool gotBlob;
		bool firstFrame;
		double totalerr;
		CvKalman* kalman_;
		CvMat* state_;
		CvMat* predictedState_;
		
		Circle currentFrame;
		Circle previousFrame;
		double dt_;
		bool bMeasurement_;
};

#endif
