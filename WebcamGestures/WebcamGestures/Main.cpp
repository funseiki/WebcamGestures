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

#include <iostream>

/** Namespaces **/
using namespace cv;

void DetectFist(Mat & cameraFrame, CascadeClassifier & classifier)
{
	Point pt1, pt2;
	vector<Rect> detectedObjects;
	double scaleFactor = 1.1;
	classifier.detectMultiScale(cameraFrame, detectedObjects, scaleFactor, CV_HAAR_DO_CANNY_PRUNING);
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

void CameraLoop()
{
	// Cascade files
	String fistClassifier = "Classifiers\\fist.xml";

	// 2.0 Api
	VideoCapture camera;
	CascadeClassifier cascade;

	// Load the classifier
	if(!cascade.load(fistClassifier))
	{
		std::cout << "Classifier " << fistClassifier << " failed to load." << std::endl;
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

		flip(cameraFrame, displayedFrame, 1);
		DetectFist(displayedFrame, cascade);

		// Display the interesting thing
		imshow("Cam", displayedFrame);

		char keypress = waitKey(20);
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