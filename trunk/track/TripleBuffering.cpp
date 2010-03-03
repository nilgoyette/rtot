#include "TripleBuffering.h"

TripleBuffering::TripleBuffering(CvSize s) 
: ready(false)
{
	this->buffer1 = cvCreateImage(s,8,3);
    this->buffer2 = cvCreateImage(s,8,3);
	this->buffer3 = cvCreateImage(s,8,3);
}

TripleBuffering::~TripleBuffering(void)
{
	
}

void TripleBuffering::write(IplImage *frame)
{
    cvCopyImage(frame,buffer1);
	{
		boost::lock_guard<boost::mutex> lock(mutswap);
		std::swap(buffer1 ,buffer2);
	}
	ready=true;
	cond.notify_one();
}

IplImage * TripleBuffering::read()
{
	//wait for fresh data
	boost::unique_lock<boost::mutex> lock(mutcond);
	while(!ready)
	{
		cond.wait(lock);
	}
	ready = false;

    {
	  // prevent writer from swapping p2 with p1 
      boost::lock_guard<boost::mutex> lock(mutswap);
	  std::swap(buffer2,buffer3); // fresh data from p2 maintenant dans p3 
	}
	// process the fresh data from p3
	return buffer3;
}
