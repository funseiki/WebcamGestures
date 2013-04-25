/**********************************************
 *	Webcam Gestures using OpenCV
 *		Vinit Shah, Ashwin Shankar, Ashutosh Dhar
 *	Adapted from opencv_handdetect.cpp by Andol Li
 *
 *********************************************/

/** Includes **/
#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>

#include <iostream>

/** Namespaces **/
using namespace cv;

/** Globals **/
RNG rng(12345);

// Create an object of the background subtractor
BackgroundSubtractorMOG2 background;

void getDefects(vector<cv::Vec4i> convexityDefectsSet, vector<int> & startInds, vector<int> & endInds, vector<int> & defectInds)
{
	vector<double>depths;
	for (int defectIterator = 0; defectIterator < convexityDefectsSet.size(); defectIterator++)
	{
		int startIdx = convexityDefectsSet[defectIterator].val[0];
		int endIdx = convexityDefectsSet[defectIterator].val[1];
		int defectPtIdx = convexityDefectsSet[defectIterator].val[2];
		double depth = (double)convexityDefectsSet[defectIterator].val[3]/256.0f;  // see documentation link below why this
		depths.push_back(depth);
	}

	Scalar average_depth = mean(depths);
	double threshold = average_depth[0]/10;

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
		if(depth >= threshold)
		{
			startInds.push_back(startIdx);
			endInds.push_back(endIdx);
			defectInds.push_back(defectPtIdx);
		}
		std::cout << depth << std::endl;
	}
}

 // Find and display Convex Hull and defects
 // Input is contour of image
void findHull(Mat src )
{
	// Contours and it's heirarchy (look at OpenCV ref)
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	// Find contours
	std::cout<<"h1";
	findContours( src, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	// Convext hull is vector of points
	// Find the convex hull object for each contour, so use 2D vector
	vector< vector<Point> > hull( contours.size() );

	// Find hull of each contour (should be just 1 in this case)
	for( int i = 0; i < contours.size(); i++ )
	{
		convexHull( Mat(contours[i]), hull[i], false );
	}
	std::cout<<"h2";
	// Draw contours + hull results
	Mat drawing = Mat::zeros( src.size(), CV_8UC3 );

	// Draw the contours and their respective hulls
	for( int i = 0; i< contours.size(); i++ )
	{
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		drawContours( drawing, contours, i, Scalar(255), 1, 8, vector<Vec4i>(), 0, Point() );
		drawContours( drawing, hull, i, Scalar(255), 1, 8, vector<Vec4i>(), 0, Point() );
	}
	std::cout<<"h3";
	// Show in a window
	imshow( "Hull", drawing );

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
	std::cout<<"h3.1";
	for(int i= 0; i<contours.size(); i++)
		convexHull( Mat(contours[i]), hulls[i], false , false);
	std::cout<<"h3.2";
	// Now, the set of defects we get will be the index of a point in the contour.
	// It save space and just works!
	// In the "C" version, defects are returned as a vector of CvPoints
	for(int i= 0; i<contours.size(); i++)
		convexityDefects(contours[i], hulls[i], convexityDefectsSet);
	std::cout<<"h4";

	vector<int> startInds;
	vector<int> endInds;
	vector<int> defectInds;

	// Each index is a point in the contour.
	// Since I know there is only 1 contour... 1st index is 0.
	getDefects(convexityDefectsSet, startInds, endInds, defectInds);

	for(int i = 0; i<startInds.size(); i++)
	{
		// Draw a yellow line from start to defect
		line(drawing, contours[0][startInds[i]], contours[0][defectInds[i]], Scalar(0,255,255), 1);

		// Draw start point (Green)
		circle(drawing, contours[0][startInds[i]], 10, Scalar(0,255,0));

		// Draw end point (Blue)
		circle(drawing, contours[0][endInds[i]], 10, Scalar(255,255,255));

		// Draw defect point (Red)
		circle(drawing, contours[0][defectInds[i]], 5, Scalar(0,0,255));
	}

	// Show and save!
	std::cout<<"h5";
	imshow("Defects", drawing);
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

bool detectHand(Mat src)
{
	std::cout << "1" << std::endl;
	Mat back;	// background frame
	Mat fore;	// foreground segment
	Mat frame(src.size(), CV_8UC3);
	src.copyTo(frame);

	// Contour points
	std::vector<std::vector<cv::Point> > foregroundContours;

	std::cout << "2" << std::endl;
	// Feed subtractor new frame and get foreground
	background.operator ()(frame,fore);

	std::cout << "3" << std::endl;
	// Get the background. History is colleceted by background object
	background.getBackgroundImage(back);
	//imshow("bg", back);

	std::cout << "4" << std::endl;
	// Erode and Dilate to remove noise (seems to work)
	erode(fore,fore,cv::Mat());
	dilate(fore,fore,cv::Mat());

	//imshow("fg", fore);
	std::cout << "5" << std::endl;
	// Find and draw Contours
	findContours(fore,foregroundContours, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	int biggestForeground = findBiggestContour(foregroundContours);

	std::cout << "6" << std::endl;
	if(biggestForeground == -1) return false;
	Rect foregroundBound = boundingRect(foregroundContours[biggestForeground]);

	std::cout << "7" << std::endl;
	// Get the region of interest
	Mat ROI = src(foregroundBound);

	std::cout << "8" << std::endl;
	// Blur image (Helps the skin detection)
	// Blur window size 5x5
	blur( ROI, ROI, Size(5,5) );

	std::cout << "9" << std::endl;
	// Convert to HSV
	Mat hsv;
	cvtColor(ROI, hsv, CV_BGR2HSV);

	// Threshold for skin.
	// LOT of trial-and-error here
	Mat bw;
	inRange(hsv, Scalar(0, 10, 170), Scalar(50, 255, 255), bw);

	// Vector of points for contours
	vector<vector<Point> > contours;

	// Hierarchy for contour detection
	vector<Vec4i> hierarchy;

	// Find contours
	findContours( bw, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	std::cout << "10" << std::endl;
	// Find the largest contour (assume, hand is largest)
	int s = findBiggestContour(contours);

	std::cout << "11" << std::endl;
	// Draw and fill detector contour
	Mat drawing = Mat::zeros( fore.size(), CV_8UC1 );
	drawContours( drawing, contours, s, Scalar(255), -1, 8, hierarchy, 0, Point());

	std::cout << "12" << std::endl;
	//Show the contour
	imshow("drw", drawing);

	// Detect hull and draw it
	findHull(drawing);

	std::cout << "13" << std::endl;
	return true;
}

void CameraLoop()
{

	// 2.0 Api
	VideoCapture camera;

	// Open the camera
	camera.open(0);
	if(!camera.isOpened())
	{
		std::cerr << "ERROR: NO CAMERA AVAILABLE!?" << std::endl;
		return;
	}

	// Set camera parameters
	camera.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	camera.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

	while(true)
	{
		Mat cameraFrame;

		// Gets the current camera frame
		camera >> cameraFrame;

		if(cameraFrame.empty())
		{
			std::cerr << "ERROR: NO CAMERA FRAME!?" << std::endl;
			exit(1);
		}

		// Output image to be drawn onto
		Mat displayedFrame(cameraFrame.size(), CV_8UC3);

		cameraFrame.copyTo(displayedFrame);
		if(!detectHand(displayedFrame)) continue;

		// Display the interesting thing
		imshow("Cam", displayedFrame);

		char keypress = waitKey(33);
		if(keypress == 27)
		{
			break;
		}
	}
}

int SingleImageTest(std::string filename)
{
	// Open image
	Mat src = imread(filename);
	if (src.empty())
	{
		std::cerr<<"Can't open file";
		return -1;
	}

	detectHand(src);
	waitKey(0);
}

int main()
{
	//CameraLoop();
	SingleImageTest("hand_pic.jpg");

}
