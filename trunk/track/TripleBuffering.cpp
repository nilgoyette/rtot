
#include "TripleBuffering.h"

using namespace std;

TripleBuffering::TripleBuffering(const CvSize s, bool blocking) throw()
		: s_(s), ready_(false), blocking_(blocking) {
	buffer1_ = cvCreateImage(s, IPL_DEPTH_8U, 3);
    buffer2_ = cvCreateImage(s, IPL_DEPTH_8U, 3);
	buffer3_ = cvCreateImage(s, IPL_DEPTH_8U, 3);
}

TripleBuffering::~TripleBuffering(void) throw() {
	cvReleaseImage(&buffer1_);
	cvReleaseImage(&buffer2_);
	cvReleaseImage(&buffer3_);
}

void TripleBuffering::write(const IplImage* const frame, DWORD time) throw() {
	if(frame->width > s_.width || frame->height > s_.height)
	{
		cvResize(frame,buffer1_);
	} else {
		cvCopyImage(frame, buffer1_);
	}
    time1_ = time;
	{
		boost::lock_guard<boost::mutex> lock(mutswap_);
		std::swap(buffer1_, buffer2_);
        std::swap(time1_, time2_);
	}
	ready_ = true;
	cond_.notify_one();
}

TimedImage TripleBuffering::read() throw() {
	if (blocking_) {
		boost::unique_lock<boost::mutex> lock(mutcond_);
		while (!ready_) { 
			cond_.wait(lock);
		}
	}
	if (!ready_) {
		return TimedImage(buffer3_,time3_);
	}

	ready_ = false;
	{
		// Prevent writer from swapping p2 with p1 
		boost::lock_guard<boost::mutex> lock(mutswap_);
		swap(buffer2_, buffer3_); // fresh data from p2 now ins p3 
        swap(time2_,time3_);
	}
	return TimedImage(buffer3_,time3_);
}

TimedImage TripleBuffering::read(int milli) throw() {
	if (blocking_) {
		boost::unique_lock<boost::mutex> lock(mutcond_);
		while (!ready_) { 
			boost::system_time timeout = boost::get_system_time() + boost::posix_time::milliseconds(milli);
			bool timedout = cond_.timed_wait(lock, timeout);
			if (!timedout) {
				return TimedImage(buffer3_,time3_);
			}
		}
	}

	if (!ready_) {
		return TimedImage(buffer3_, time3_);
	}

	ready_ = false;
	{
		// Prevent writer from swapping p2 with p1 
		boost::lock_guard<boost::mutex> lock(mutswap_);
		swap(buffer2_, buffer3_); // fresh data from p2 now ins p3
        swap(time2_, time3_);
	}
	return TimedImage(buffer3_,time3_);
}