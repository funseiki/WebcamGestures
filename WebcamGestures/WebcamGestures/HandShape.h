#pragma once
#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>
using namespace cv;

class HandShape
{
public:
	HandShape(void);
	~HandShape(void);
private:
	int _fingerCount;
	int _angle;
	Point _centroid;
};