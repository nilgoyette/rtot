
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

#include <iostream>

#include "cv.h"

class Circle {
	public:
		float radius_;
		float x_;
		float y_;
		
		bool empty_;

		Circle(void) : empty_(true) {}
		Circle(float x, float y, float radius) {
			init(x, y, radius);
		}

		~Circle() {}

		Circle(const CvRect bounds) {
			init(bounds);
		}

		CvPoint getCenter() const throw() {
			return cvPoint(static_cast<int>(x_ + 0.5f), static_cast<int>(y_ + 0.5f));
		}
		
		float distance(Circle& other) const throw() {
			return sqrt(pow(x_ - other.x_, 2) + pow(y_ - other.y_, 2));
		}

		void init(float x, float y, float radius) throw() {
			x_ = x;
			y_ = y;
			radius_ = radius;
			empty_ = false;
		}

		void init(const CvRect bounds) throw() {
			const unsigned int height = bounds.height;
			const unsigned int width = bounds.width;
			init(static_cast<float>(bounds.x) + (static_cast<float>(width) / 2.0f),
				 static_cast<float>(bounds.y) + (static_cast<float>(height) / 2.0f),
				 static_cast<float>(std::max(height, width)) / 2.0f);
		}

	private:
		CvRect bounds_;
		CvPoint center_;
};

#endif
