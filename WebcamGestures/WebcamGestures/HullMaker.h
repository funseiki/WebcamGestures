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
	HullMaker(Mat input, BackgroundSubtractorMOG2 & bgSub, double learningRate);
	~HullMaker(void);

	// Main method
	void MakeHull(Mat input);

	// Getter methods
	Mat getHullImage();
	void getDefectPoints(vector<Point> & starts, vector<Point> & ends, vector<Point> & defects);
	bool isValidHull();
	float getHullLength();

private:
	// Members
	Mat image, inputImage;
	RNG rng;
	bool isValid;
	vector<Point> startPoints;
	vector<Point> endPoints;
	vector<Point> defectPoints;
	vector< vector<Point> > contours;
	BackgroundSubtractorMOG2 bgSubtractor;

	// Threshold Ratio: A magic number.
	//		Found that a threshold of ~10 for a hull of ~465-500 produced good results
	double thresholdRatio;
	float hullLength;

	// Methods
	bool findHull(vector<vector<Point> > contours, Mat input);
	int findLargestContour(vector<vector<Point> > contours, unsigned int & sizeOfBiggestContour);
	Mat erodeAndDilate(Mat input);
	Mat subtractBackground(Mat input, BackgroundSubtractorMOG2 & bgSub, double learningRate);
	Mat preprocess(Mat input);
	bool getDefectsSet(vector<vector<Point> > contours, vector<cv::Vec4i> & defectsSet);
	void getDefects(vector<cv::Vec4i> convexityDefectsSet, vector<Point> contour,
					vector<Point> & starts, vector<Point> & ends,
					vector<Point> & defects, double threshold);
	Mat buildImage(vector<vector<Point> > contours, Mat input);

	// Defaults
	void setDefaults();
};
