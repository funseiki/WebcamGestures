#include "HullMaker.h"


HullMaker::HullMaker(void)
{
	setDefaults();
}

HullMaker::HullMaker(Mat input)
{
	inputImage = input;
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
	image(Rect(0,0,0,0));
	thresholdRatio = 10.0/500.0;
}

// Getters
Mat HullMaker::getHullImage()
{
	// If we haven't built the image already and the image is valid
	if(image.size().height == 0 && isValid)
	{
		image = buildImage(contours, inputImage);
	}
	return image;
}

void HullMaker::getDefectPoints(vector<Point> & starts, vector<Point> & ends, vector<Point> & defects)
{
	starts = startPoints;
	ends = endPoints;
	defects = defectPoints;
}

bool HullMaker::isValidHull()
{
	return isValid;
}

float HullMaker::getHullLength()
{
	return hullLength;
}

// Main method
void HullMaker::MakeHull(Mat input)
{
	vector<Vec4i> hierarchy;
	Mat cleanImage;

	// Format the input into a usable image
	cleanImage = preprocess(input);

	// Find contours
	findContours(cleanImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	// Fit a hull using these contour points
	isValid = findHull(contours, input);
}

// Helper methods

void HullMaker::getDefects(vector<cv::Vec4i> convexityDefectsSet, vector<Point> contour,
					vector<Point> & starts, vector<Point> & ends,
					vector<Point> & defects, double threshold = 10)
{
	// First figure out what all the depths are
	vector<double>depths;
	for (unsigned int defectIterator = 0; defectIterator < convexityDefectsSet.size(); defectIterator++)
	{
		double depth = (double)convexityDefectsSet[defectIterator].val[3]/256.0f;  // see documentation link below why this
		depths.push_back(depth);
	}

	Scalar average_depth = mean(depths);

	//average_depth.val[0]/8;

	vector<double>dists;
	// defectIterator (Java style, to keep Ashwin happy! ;p
	// For each defect found
	for (unsigned int defectIterator = 0; defectIterator < convexityDefectsSet.size(); defectIterator++)
	{
		// Get the indices from the vector structure returned
		// Each defect has a start, end and deep point
		// Also, depth from start to the deep point
		int startIdx = convexityDefectsSet[defectIterator].val[0];
		int endIdx = convexityDefectsSet[defectIterator].val[1];
		int defectPtIdx = convexityDefectsSet[defectIterator].val[2];
		double depth = (double)convexityDefectsSet[defectIterator].val[3]/256.0f;  // see documentation link below why this

		if(depth >= threshold)
		{
			starts.push_back(contour[startIdx]);
			ends.push_back(contour[endIdx]);
			defects.push_back(contour[defectPtIdx]);
			double distx = (contour[startIdx].x - contour[endIdx].x)*(contour[startIdx].x - contour[endIdx].x);
			double disty = (contour[startIdx].y - contour[endIdx].y)*(contour[startIdx].y - contour[endIdx].y);
			double dist = sqrt(distx+disty);
			dists.push_back(dist);
		}
	}
}


bool HullMaker::findHull(vector<vector<Point> > contours, Mat input)
{
	// Set of convexity defects
	vector<cv::Vec4i> defectsSet(contours.size());

	// If we have an invalid contour, there's nothing we can do
	if(contours.size() < 1 || contours[0].size() <= 3)
	{
		return false;
	}

	// Get the convexity defects set
	if(!getDefectsSet(contours, defectsSet))
	{
		return false;	// Invalid set
	}

	Point2f center;
	float radius;
	// Determine size of the hull
	minEnclosingCircle(contours[0], center, radius);
	hullLength = radius*2;

	// Set the defect points, we're assuming that we only have the one contour
	getDefects(defectsSet, contours[0], startPoints, endPoints, defectPoints, hullLength*thresholdRatio);
	if(startPoints.size() <= 3)
	{
		return false;
	}
	return true;
}

Mat HullMaker::buildImage(vector<vector<Point> > contours, Mat input)
{
	vector< vector<Point> > hull(contours.size());
	Mat drawing(input.size(), CV_8UC3);

	// Find hull of each contour (should be just 1 in this case)
	for(unsigned int i = 0; i < contours.size(); i++ )
	{
		convexHull( Mat(contours[i]), hull[i], false );
	}

	// Draw the contours and their respective hulls
	for(unsigned int i = 0; i< contours.size(); i++ )
	{
		drawContours( drawing, contours, i, Scalar(255), 1, 8, vector<Vec4i>(), 0, Point() );
		drawContours( drawing, hull, i, Scalar(255), 1, 8, vector<Vec4i>(), 0, Point() );
	}

	return drawing;
}

bool HullMaker::getDefectsSet(vector<vector<Point> > contours, vector<cv::Vec4i> & defectsSet)
{
	/* Each Defect is a vector of 4 elements
	 * It has the start, end and most "deep" point (Refer to documentation)
	 * Also has depth from start to the deep point
	 * Each hull can have multiple defects, so we need a vector of this vectors (one per hull)
	 * Size of hull vector is size of contour */
	vector < vector<int> > hulls(contours.size());

	// Find the hull again... This time as vector of integers.
	// Each element is an "index in the corresponding contour level"
	// Need this to get the defects portion working
	for(unsigned int i = 0; i < contours.size(); i++)
	{
		convexHull( Mat(contours[i]), hulls[i], false , false);
	}

	// Now, the set of defects we get will be the index of a point in the contour.
	// It saves space and just works!
	for(unsigned int i= 0; i < contours.size(); i++)
	{
		try
		{
			convexityDefects(contours[i], hulls[i], defectsSet);
		}
		catch(Exception e)
		{
			std::cout << e.what() << std::endl;
			return false;
		}
	}

	return true;
}

int HullMaker::findLargestContour(vector<vector<Point> > contours)
{
	// Index of largest contour
	int indexOfBiggestContour = -1;

	// Size of contour (temp var)
	unsigned int sizeOfBiggestContour = 0;

	// Iterate through each contour
	for (unsigned int i = 0; i < contours.size(); i++)
	{
		// Pretty straigh forward! Find the largest contour
		if(contours[i].size() > sizeOfBiggestContour)
		{
			sizeOfBiggestContour = contours[i].size();
			indexOfBiggestContour = i;
		}
	}

	return indexOfBiggestContour;
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
