
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

#include <iostream>

#include "cv.h"

class Circle {
	public:
		unsigned int radius_;
		CvPoint center_;
		CvRect bounds_;
		bool empty_;

		Circle(void) : empty_(true) { }
		Circle(const CvRect bounds) {
			init(bounds);
		}

		void init(const CvRect bounds) {
			const unsigned int height = bounds.height;
			const unsigned int width = bounds.width;
			center_.x = bounds.x + (width / 2);
			center_.y = bounds.y + (height / 2);
			radius_ = std::max(height, width) / 2;
			empty_ = false;
		}
};

#endif
