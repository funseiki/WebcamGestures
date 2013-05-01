#include "HullMaker.h"


HullMaker::HullMaker(void)
{
	setDefaults();
}

HullMaker::HullMaker(Mat input)
{
	setDefaults();
	MakeHull(input);
}

HullMaker::~HullMaker(void)
{
}

void HullMaker::setDefaults()
{
	rng = RNG(12345);
	isValid = false;
}

// Getters
Mat HullMaker::getHullImage()
{
	return image;
}

// Main method
void HullMaker::MakeHull(Mat input)
{
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat cleanImage;

	cleanImage = preprocess(input);

	// Find contours
	findContours(cleanImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	// Fit a hull using these contour points
	findHull(contours, hierarchy, input);
}

// Helper methods
void HullMaker::findHull(vector<vector<Point>> contours, vector<Vec4i> hierarchy, Mat input)
{
	vector< vector<Point> > hull( contours.size() );
	Mat drawing(input.size(), CV_8UC3);

	// Find hull of each contour (should be just 1 in this case)
	for( int i = 0; i < contours.size(); i++ )
	{
		convexHull( Mat(contours[i]), hull[i], false );
	}

	// Draw the contours and their respective hulls
	for( int i = 0; i< contours.size(); i++ )
	{
		drawContours( drawing, contours, i, Scalar(255), 1, 8, vector<Vec4i>(), 0, Point() );
		drawContours( drawing, hull, i, Scalar(255), 1, 8, vector<Vec4i>(), 0, Point() );
	}

	image = drawing;
}

int HullMaker::findLargestContour(vector<vector<Point>> contours)
{
	return 0;
}

Mat HullMaker::preprocess(Mat input)
{
	Mat current, hsv, bw;

	// Convert to grayscale
	cvtColor(input,current,CV_BGR2GRAY);

	// Preprocess the image
	current = erodeAndDilate(subtractBackground(current));

	// Convert to HSV
	cvtColor(input, hsv, CV_BGR2GRAY);

	// Convert to black and white
	inRange(hsv, Scalar(50), Scalar(255), bw);

	return bw;
}

Mat HullMaker::erodeAndDilate(Mat input)
{
	// TODO: Implement this
	return input;
}

Mat HullMaker::subtractBackground(Mat input)
{
	// TODO: Implement this
	return input;
}
