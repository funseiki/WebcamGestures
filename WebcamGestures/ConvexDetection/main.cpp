 #include "opencv2/highgui/highgui.hpp"
 #include "opencv2/imgproc/imgproc.hpp"
 #include <iostream>
 #include <stdio.h>
 #include <stdlib.h>

 using namespace cv;
 using namespace std;

 RNG rng(12345);

 // Find and display Convex Hull and defects
 // Input is contour of image
void hull(Mat src )
 {
	// Make local copy (Not really needed, but it doesn't kill to be safe!)
	Mat src_copy = src.clone();

	// Contours and it's heirarchy (look at OpenCV ref)
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	// Find contours
	findContours( src, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	// Convext hull is vector of points
	// Find the convex hull object for each contour, so use 2D vector
	vector< vector<Point> > hull( contours.size() );

	// Find hull of each contour (should be just 1 in this case)
	for( int i = 0; i < contours.size(); i++ )
	{
		convexHull( Mat(contours[i]), hull[i], false );
	}

	// Draw contours + hull results
	Mat drawing = Mat::zeros( src.size(), CV_8UC3 );

	// Draw the contours and their respective hulls
	for( int i = 0; i< contours.size(); i++ )
	{
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		drawContours( drawing, contours, i, Scalar(255), 1, 8, vector<Vec4i>(), 0, Point() );
		drawContours( drawing, hull, i, Scalar(255), 1, 8, vector<Vec4i>(), 0, Point() );
	}

	// Show in a window
	imshow( "Hull", drawing );
	waitKey(0);

	// To find the "defects in the hull"

	// Each Defect is a vector of 4 elements
	// It has the start, end and most "deep" point (Refer to documentation)
	// Also has depth from start to the deep point
	// Each hull can have multiple defects, so we need a vector of this vectors (one per hull)
	// Size of hull vector is size of contour
	vector<cv::Vec4i> convexityDefectsSet(contours.size());

	// THIS IS THE TRICKY PART HERE!

	// Find the hull again... This time as vector of integers.
	// Each element is an "index in the corresponding contour level"
	// Need this to get the defects portion working
	vector < vector<int> > hulls(contours.size());

	// I've called the convexHull function with a 4th parameter, explicitly telling it NOT to return points
	// Instead indices are returned
	for(int i= 0; i<contours.size(); i++)
		convexHull( Mat(contours[i]), hulls[i], false , false);

	// Now, the set of defects we get will be the index of a point in the contour.
	// It save space and just works!
	// In the "C" version, defects are returned as a vector of CvPoints
	for(int i= 0; i<contours.size(); i++)
		convexityDefects(contours[i], hulls[i], convexityDefectsSet);

	// defectIterator (Java style, to keep Ashwin happy! ;p
	// For each defect found
	for (int defectIterator = 0; defectIterator < convexityDefectsSet.size(); defectIterator++)
	{
		// Get the indices from the vector structure returned
		// Each defect has a start, end and deep point
		// Also, depth from start to the deep point
		int startIdx = convexityDefectsSet[defectIterator].val[0];
		int endIdx = convexityDefectsSet[defectIterator].val[1];
		int defectPtIdx = convexityDefectsSet[defectIterator].val[2];
		double depth = (double)convexityDefectsSet[defectIterator].val[3]/256.0f;  // see documentation link below why this

		// Each index is a point in the contour.
		// Since I know there is only 1 contour... 1st index is 0.

		// Draw a yellow line from start to defect
		line(drawing, contours[0][startIdx], contours[0][defectPtIdx], Scalar(0,255,255), 1);

		// Draw a red line from start to end
		//line(drawing, contours[0][startIdx], contours[0][endIdx], Scalar(0,0,255), 1);

		// Draw start point (Green)
		circle(drawing, contours[0][startIdx], 10, Scalar(0,255,0));

		// Draw end point (Blue)
		circle(drawing, contours[0][endIdx], 10, Scalar(255,0,0));

		// Draw defect point (Red)
		circle(drawing, contours[0][defectPtIdx], 5, Scalar(0,0,255));

	}

	// Show and save!
	imshow("Defects", drawing);
	imwrite("ConvexHull.jpg", drawing);
	waitKey(0);
 }

 // Returns index of largest vector
int findBiggestContour( vector < vector<Point> > contours)
{
// Keep in mind, a contour is a vector of points
// So multiple contours are 2D vectors of points

	// Index of largest contour
	int indexOfBiggestContour = -1;

	// Size of contour (temp var)
	int sizeOfBiggestContour = 0;

	// Iterate through each contour
	for (int i = 0; i < contours.size(); i++)
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


int main(int argc, char **argv)
{

	// Open image
	Mat src = imread("fist.jpg");
	if (src.empty())
	{
		cerr<<"Can't open file";
		return -1;
	}

	// Blur image (Helps the skin detection)
	// Blur window size 5x5
	blur( src, src, Size(5,5) );

	// Convert to HSV
	Mat hsv;
	cvtColor(src, hsv, CV_BGR2HSV);

	// Threshold for skin.
	// LOT of trial-and-error here
	Mat bw;
	inRange(hsv, Scalar(0, 10, 170), Scalar(50, 255, 255), bw);

	// Show images
	imshow("src", src);
	imshow("dst", bw);

	// Vector of points for contours
	vector<vector<Point> > contours;
	// Hierarchy for contour detection
	vector<Vec4i> hierarchy;

	// Find contours
	findContours( bw, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	// Find the largest contour (assume, hand is largest)
	int s = findBiggestContour(contours);

	// Draw and fill detector contour
	Mat drawing = Mat::zeros( src.size(), CV_8UC1 );
	drawContours( drawing, contours, s, Scalar(255), -1, 8, hierarchy, 0, Point() );

	//Show the contour
	imshow("drw", drawing);

	// Detect hull and draw it
	hull(drawing);

	waitKey(0);
	return 0;
}
