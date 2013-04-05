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
using namespace std;
using namespace cv;

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
		cout << "Classifier " << fistClassifier << " failed to load." << endl;
		return;
	}

	// Open the camera
	camera.open(0);
	if(!camera.isOpened())
	{
		cerr << "ERROR: NO CAMERA AVAILABLE!?" << endl;
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
			cerr << "ERROR: NO CAMERA FRAME!?" << endl;
			exit(1);
		}

		// Output image to be drawn onto
		Mat displayedFrame(cameraFrame.size(), CV_8UC3);

		cameraFrame.copyTo(displayedFrame);

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