
#ifndef __TRIPLE_BUFFERING_H__
#define __TRIPLE_BUFFERING_H__

#include <boost/thread.hpp>

#include "cv.h"

// Boost Mutix map to a CRITICAL_SECTION on Windows

class TripleBuffering {
	public:
		TripleBuffering(const CvSize s) throw();
		~TripleBuffering(void) throw();
		void write(const IplImage* const frame) throw();
		IplImage* read() throw();

	private:
		boost::condition_variable cond_;
		boost::mutex mutswap_;
		boost::mutex mutcond_;
		bool ready_;

		IplImage *buffer1_;
		IplImage *buffer2_;
		IplImage *buffer3_;

		TripleBuffering(const TripleBuffering&);
};

#endif
