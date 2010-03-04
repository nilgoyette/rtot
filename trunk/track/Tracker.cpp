#include "Tracker.h"

Tracker::Tracker(void){
    kalman = cvCreateKalman( 2, 2, 0 );
    cvSetIdentity(kalman->measurement_matrix, cvRealScalar(1));     //identity
    cvSetIdentity(kalman->error_cov_post, cvRealScalar(1));         //identity
    cvSetIdentity(kalman->process_noise_cov, cvRealScalar(0.4));
    cvSetIdentity(kalman->measurement_noise_cov, cvRealScalar(3));
    state = cvCreateMat( 2, 1, CV_32FC1 );                          /* (phi, delta_phi) */
}

Tracker::~Tracker(void)
{
}

const Circle& Tracker::process(IplImage* backproject){
    blobs = CBlobResult( backproject, NULL, 0 );
    blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, 5000 );
    // display filtered blobs
    current.empty = true;
    if ( blobs.GetNumBlobs() > 0){
        currentBlob = blobs.GetBlob(0);
        current.bounds = currentBlob->GetBoundingBox();

        int height = current.bounds.height;
        int width = current.bounds.width;
        int max =  height > width ? height : width; 
        int x = current.bounds.x + (width/2);
        int y = current.bounds.y + (height/2);

        const CvMat* prediction = cvKalmanPredict(kalman);
        current.center = cvPoint((int)cvmGet(prediction, 0, 0),
                         (int)cvmGet(prediction, 1, 0));

        cvmSet(state, 0, 0, x);
        cvmSet(state, 1, 0, y);
        cvKalmanCorrect(kalman, state);

        current.radius = max/2;
        current.empty = false;
    }
    
     return current;
}