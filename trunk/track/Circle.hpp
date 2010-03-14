
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

	Circle(void) : empty_(true) { }
	Circle(float x,float y,float radius) {
		init(x,y,radius);
	}
	~Circle(){
	} 
	Circle(const CvRect bounds) {
		init(bounds);
	}

	CvPoint getCenter(){
		return cvPoint(int(x_+0.5f),int(y_+0.5f));
	}
	
	float distance( Circle& other) {
		return sqrt(pow(this->x_ -other.x_,2)  + pow(this->y_ - other.y_,2));
	}
	void init(float x,float y,float radius) {
		x_ = x;
		y_ = y;
		this->radius_ = radius;
		empty_ = false;
	}
	void init(const CvRect bounds) {
		const unsigned int height = bounds.height;
		const unsigned int width = bounds.width;
		init((float)bounds.x + ((float)width / 2.0f),
			 (float)bounds.y + ((float)height / 2.0f),
			 (float)std::max(height, width) / 2.0f);
	}
private:
	CvRect bounds_;
	CvPoint center_;
};

#endif
