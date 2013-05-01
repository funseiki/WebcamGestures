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

	// Main method
	void MakeHull(Mat input);

	// Getter methods
	Mat getHullImage();
	void getDefectPoints(vector<Point> & starts, vector<Point> & ends, vector<Point> & defects);
	bool isValidHull();

private:
	// Members
	Mat image, inputImage;
	RNG rng;
	bool isValid;
	vector<Point> startPoints;
	vector<Point> endPoints;
	vector<Point> defectPoints;
	vector< vector<Point> > contours;

	// Methods
	bool findHull(vector<vector<Point> > contours, Mat input);
	int findLargestContour(vector<vector<Point> > contours);
	Mat erodeAndDilate(Mat input);
	Mat subtractBackground(Mat input);
	Mat preprocess(Mat input);
	bool getDefectsSet(vector<vector<Point> > contours, vector<cv::Vec4i> & defectsSet);
	void getDefects(vector<cv::Vec4i> convexityDefectsSet, vector<Point> contour,
					vector<Point> & starts, vector<Point> & ends,
					vector<Point> & defects);
	Mat buildImage(vector<vector<Point> > contours, Mat input);

	// Defaults
	void setDefaults();
};
