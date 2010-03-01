#if !defined(__DATASTRUCTURES_H__)
#define __DATASTRUCTURES_H__


typedef struct imgData{
	bool locked;
	IplImage *hsv;
	IplImage *c_h;
	IplImage *c_s;
	IplImage *c_v;
	IplImage *backproject;
}imageData_t;

#endif
