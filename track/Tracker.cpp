
#include "Tracker.h"

Tracker::Tracker(void) {
    kalman = cvCreateKalman(2, 2, 0);
    cvSetIdentity(kalman->measurement_matrix, cvRealScalar(1));     // identity
    cvSetIdentity(kalman->error_cov_post, cvRealScalar(1));         // identity
    cvSetIdentity(kalman->process_noise_cov, cvRealScalar(0.4));
    cvSetIdentity(kalman->measurement_noise_cov, cvRealScalar(3));
    state = cvCreateMat(2, 1, CV_32FC1);							// (phi, delta_phi)
}

const Circle& Tracker::process(IplImage* backproject) throw() {
    blobs = CBlobResult(backproject, NULL, 0);
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER, 5000);

    // Display filtered blobs
    current.empty_ = true;
    if (blobs.GetNumBlobs() > 0) {
        currentBlob = blobs.GetBlob(0);
		current.init(currentBlob->GetBoundingBox());

        const CvMat* prediction = cvKalmanPredict(kalman);
        current.center_ = cvPoint((int)cvmGet(prediction, 0, 0),
								  (int)cvmGet(prediction, 1, 0));

		cvmSet(state, 0, 0, current.center_.x);
		cvmSet(state, 1, 0, current.center_.y);
        cvKalmanCorrect(kalman, state);
    }
    return current;
}
