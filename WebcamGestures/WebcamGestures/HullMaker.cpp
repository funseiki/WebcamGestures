#include "HullMaker.h"


HullMaker::HullMaker(void)
{
}

HullMaker::HullMaker(Mat input)
{
	MakeHull(input);
}

HullMaker::~HullMaker(void)
{
}

// Main method
void HullMaker::MakeHull(Mat input)
{
	rng = RNG(12345);
	image = erodeAndDilate(subtractBackground(input));
}

// Helper methods
void HullMaker::findHull()
{

}

int HullMaker::findLargestContour(vector<vector<Point>> contours)
{
	return 0;
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
