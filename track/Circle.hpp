
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

#include <iostream>

#include "cv.h"

class Circle {
public:
	int radius_;
	CvPoint center_;
	bool empty_;

	Circle(void) : empty_(true) { }
	Circle(const CvRect bounds) {
		init(bounds);
	}
	
	int distance( Circle other) {
		return (int)sqrt(pow(float(this->center_.x -other.center_.x),2)  + pow(float(this->center_.y -other.center_.y),2));
	}

	void init(const CvRect bounds) {
		const unsigned int height = bounds.height;
		const unsigned int width = bounds.width;
		center_.x = bounds.x + (width / 2);
		center_.y = bounds.y + (height / 2);
		radius_ = std::max(height, width) / 2;
		empty_ = false;
	}
private:
	CvRect bounds_;
};

#endif
