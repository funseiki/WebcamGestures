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

void DetectClassifers(Mat & cameraFrame, vector<CascadeClassifier> & classifiers)
{
	Point pt1, pt2;
	double scaleFactor = 1.1;

	// Iterate over each classifier and see if we find anything
	for(vector<CascadeClassifier>::iterator it = classifiers.begin(); it != classifiers.end();
			it++)
	{
		vector<Rect> detectedObjects;
		it->detectMultiScale(cameraFrame, detectedObjects, scaleFactor, CV_HAAR_DO_CANNY_PRUNING);

		// Draw a rectangle around the detected objects
		for(int i = 0; i < detectedObjects.size(); i++)
		{
			Rect currentHand = detectedObjects[i];
			// Set the bounding box corners
			pt1.x = currentHand.x * scaleFactor;
			pt2.x = (currentHand.x + currentHand.width)*scaleFactor;
			pt1.y = currentHand.y*scaleFactor;
			pt2.y = (currentHand.y + currentHand.height)*scaleFactor;

			// Draw the rectangle around the detected hand
			rectangle(cameraFrame, pt1, pt2, CV_RGB(200, 0, 0), 1, 8, 0);
		}
	}

}

int loadClassifiers(vector<CascadeClassifier> & classifiers)
{
	// Cascade files
	String fistClassifier = "Classifiers\\fist.xml";
	String palmClassifer = "Classifiers\\palm.xml";

	vector<String> files;
	files.push_back(fistClassifier);
	files.push_back(palmClassifer);

	for(vector<String>::iterator it = files.begin();
		it != files.end(); it++)
	{
		CascadeClassifier c;
		// Load the classifier
		if(!c.load(*it))
		{
			std::cout << "Classifier " << *it << " failed to load." << std::endl;
			return 0;
		}
		classifiers.push_back(c);
	}

	return 1;
}

 // Find and display Convex Hull and defects
 // Input is contour of image
void hull(Mat src )
{
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

void detectHand(Mat src)
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
	imshow("bg", back);

	std::cout << "4" << std::endl;
	// Erode and Dilate to remove noise (seems to work)
	erode(fore,fore,cv::Mat());
	dilate(fore,fore,cv::Mat());

	imshow("fg", fore);
	std::cout << "5" << std::endl;
	// Find and draw Contours
	findContours(fore,foregroundContours, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	int biggestForeground = findBiggestContour(foregroundContours);

	std::cout << "6" << std::endl;
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
	hull(drawing);

	std::cout << "13" << std::endl;
}

void CameraLoop()
{

	// 2.0 Api
	VideoCapture camera;

	vector<CascadeClassifier> classifiers;
	if(!loadClassifiers(classifiers))
	{
		return;
	}

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
		detectHand(displayedFrame);
		//DetectClassifers(displayedFrame, classifiers);

		// Display the interesting thing
		imshow("Cam", displayedFrame);

		char keypress = waitKey(33);
		if(keypress == 27)
		{
			break;
		}
	}
}

int main()
{
	CameraLoop();
}