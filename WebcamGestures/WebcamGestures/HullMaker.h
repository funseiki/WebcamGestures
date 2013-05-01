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

	Mat getHullImage();

private:
	// Members
	Mat image;
	RNG rng;
	bool isValid;
	vector<Point> startPoints;
	vector<Point> endPoints;
	vector<Point> defectPoints;

	// Methods
	void findHull(vector<vector<Point>> contours,vector<Vec4i> hierarchy, Mat input);
	int findLargestContour(vector<vector<Point>> contours);
	Mat erodeAndDilate(Mat input);
	Mat subtractBackground(Mat input);
	Mat preprocess(Mat input);
	void MakeHull(Mat input);

	// Defaults
	void setDefaults();
};

