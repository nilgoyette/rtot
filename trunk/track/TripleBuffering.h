#pragma once
#include <boost/thread.hpp>
#include "cv.h"
class TripleBuffering
{
public:
	//will map to a CRITICAL_SECTION on Windows
	boost::mutex mutswap;
    boost::mutex mutcond;
	bool ready;
    boost::condition_variable cond;
	IplImage *buffer1;
	IplImage *buffer2;
	IplImage *buffer3;

	TripleBuffering(CvSize s);
	~TripleBuffering(void);
	void write(IplImage *frame);
	IplImage * read();
};
