
#ifndef __GRABBER_H__
#define __GRABBER_H__

#include <boost/thread.hpp>

#include "cv.h"
#include "highgui.h"

#include "TripleBuffering.h"
#include "Uncopiable.h"

class Grabber : Uncopiable<> {
    public:
        Grabber(const int, const CvSize, TripleBuffering&) throw();
        ~Grabber(void) throw();
        
        void exit();
        void grabber(void) throw();
        void operator()() throw();
        virtual void beforeGrab() throw();

    private:
        const int camId_;       //  
        CvCapture* capture_;    // Image acquisition object
        bool initialized_;      // 
        TripleBuffering& sink_; // Concurrent image queue

        IplImage *image_;       // 

        Grabber(const Grabber&);
        bool exit_;
};

#endif
