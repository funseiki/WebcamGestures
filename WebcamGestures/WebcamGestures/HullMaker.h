#pragma once
#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

class HullMaker
{
public:
	HullMaker(void);
	HullMaker(Mat input);
	~HullMaker(void);

private:
	// Members
	Mat image;
	RNG rng;

	// Methods
	void findHull();
	int findLargestContour(vector<vector<Point>> contours);
	Mat erodeAndDilate(Mat input);
	Mat subtractBackground(Mat input);
	void MakeHull(Mat input);
};

