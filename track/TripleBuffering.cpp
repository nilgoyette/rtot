
#include "TripleBuffering.h"

using namespace std;

TripleBuffering::TripleBuffering(const CvSize s,bool blocking) throw()
		: ready_(false),
		  blocking_(blocking) {
	// TODO : Trouver l'enum pour 8 et 3
	buffer1_ = cvCreateImage(s, 8, 3);
    buffer2_ = cvCreateImage(s, 8, 3);
	buffer3_ = cvCreateImage(s, 8, 3);
}

TripleBuffering::~TripleBuffering(void) throw() {
	cvReleaseImage(&buffer1_);
	cvReleaseImage(&buffer2_);
	cvReleaseImage(&buffer3_);
}

void TripleBuffering::write(const IplImage* const frame) throw() {
    cvCopyImage(frame, buffer1_);
	{
		boost::lock_guard<boost::mutex> lock(mutswap_);
		std::swap(buffer1_, buffer2_);
	}
	ready_ = true;
	cond_.notify_one();
}

IplImage* TripleBuffering::read() throw() {
	if (blocking_){
		boost::unique_lock<boost::mutex> lock(mutcond_);
		while(!ready_) {
			cond_.wait(lock);
		}
	}else{
		if(!ready_){
			return buffer3_;
		}
	}
	ready_ = false;
	{
		// Prevent writer from swapping p2 with p1 
		boost::lock_guard<boost::mutex> lock(mutswap_);
		swap(buffer2_, buffer3_); // fresh data from p2 now ins p3 
	}

	return buffer3_;
}
