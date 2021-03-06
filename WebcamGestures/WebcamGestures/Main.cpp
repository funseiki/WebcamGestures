/**********************************************
 *	Webcam Gestures using OpenCV
 *		Vinit Shah, Ashwin Shankar, Ashutosh Dhar
 *	Adaptaption from algorithms proposed/used by this guy
 *		https://www.youtube.com/watch?v=B4dwu3si9x0
 *********************************************/

/** Includes **/
#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

#include "HandShape.h"
#include "HullMaker.h"
#include "Utils.h"

/** Namespaces **/
using namespace cv;

int BUFFER_SIZE = 6;

std::vector<HandShape> hands;
int state = 0;//state = 0 implies SEARCHING and state = 1 implies RECOGNITION
HandShape startPoint;//stores the start point of our gesture search
Point2f averageStartCentroid;
Point2f averageEndCentroid;
HandShape previousHand;
int countStillFrame = 0;

// Classify gesture detected
void getGesture(){
		float angle = findAngleBetween(averageStartCentroid, averageEndCentroid);//atan2(averageStartCentroid.y - averageEndCentroid.y, averageStartCentroid.x - averageEndCentroid.x);
		
		int direction = classifyMotion(angle,0.3,true,true);
		
		String directionStr = "";
		if(direction == 0) directionStr = "UP";
		else if(direction == 1) directionStr = "DOWN";
		else if(direction == 2) directionStr = "RIGHT";
		else directionStr = "LEFT";
		
		std::cout << "Gesture finger count : " << startPoint.getfingerCount();
		std::cout << "  Motion : "<<directionStr<< std::endl;
		//waitKey(0);
}

// Set the global centroid from our frame buffer
void setAverageCentroid(){

	float totalX = 0;
	float totalY = 0;
	for(int handsIterator = 0; handsIterator < hands.size(); handsIterator++) {
			HandShape hp = hands[handsIterator];
			totalX += hp.getCentroid().x;
			totalY += hp.getCentroid().y;
	}
	if(state == 0) {
		averageStartCentroid.x = totalX/BUFFER_SIZE;
		averageStartCentroid.y = totalY/BUFFER_SIZE;
	} else {
		averageEndCentroid.x = totalX/BUFFER_SIZE;
		averageEndCentroid.y = totalY/BUFFER_SIZE;
	}
}

// Find our gesture
void detectGestureByMotion(HandShape currentHand){
	if(state == 0) { //Not started tracking 
		int dist = findDistanceBetween(currentHand.getCentroid(),previousHand.getCentroid());
		if(dist < 10) {
			countStillFrame++;
			if(countStillFrame == 5){
				setAverageCentroid();
				state = 1;		
				std::cout <<"Tracking started"<<std::endl;
				startPoint = currentHand;
			}
		} else {
			countStillFrame = 0;
		}
	}
	hands.push_back(currentHand);
	if(hands.size() > BUFFER_SIZE) hands.erase(hands.begin());
}

// Main loop
void CameraLoop(std::string filename = "", std::string outfile = "")
{
	// 2.0 Api
	VideoCapture camera;
	int history = 300;
	float learningRate = 0.0001;
	bool subtractBackground = false;

	// Create an object of the background subtractor
	BackgroundSubtractorMOG2 background(history,16,false);

	// Camera Parameters
	int ex = CV_FOURCC('P', 'I', 'M', '1');
	Size S = Size(640, 480);
	double framerate = 20;

	// Open the camera
	if(filename.length() > 0)
	{
		camera.open(filename);
		ex = static_cast<int>(camera.get(CV_CAP_PROP_FOURCC));
		S = Size((int) camera.get(CV_CAP_PROP_FRAME_WIDTH),    //Acquire input size
				  (int) camera.get(CV_CAP_PROP_FRAME_HEIGHT));
		framerate = camera.get(CV_CAP_PROP_FPS);
	}
	else
	{
		subtractBackground = true;
		camera.open(0);
	}
	if(!camera.isOpened())
	{
		std::cerr << "ERROR: NO CAMERA AVAILABLE!?" << std::endl;
		return;
	}

	VideoWriter writer;
	if(outfile.length() > 0)
	{
		writer.open(outfile, ex, framerate, S, true);
		if(!writer.isOpened())
		{
			std::cerr << "Output video not available" << std::endl;
			return;
		}
	}

	// Set camera parameters
	int totalHandCount = 0;
	int validHandCount = 0;
	
	while(true)
	{
		// Use this for tracking
		totalHandCount++;
		if((totalHandCount - validHandCount) > 10 && state == 1){
			setAverageCentroid();
			getGesture();
			state = 0;
			std::cout <<"Tracking stopped"<<std::endl;
			previousHand.~HandShape();
			new(&previousHand) HandShape();
		}

		// Image matrices for display purposes
		Mat cameraFrame, mirror, mirrorHand;

		// Gets the current camera frame
		camera >> cameraFrame;

		// Foreground matrix to display detected foreground contours
		Mat fg;
		resize(cameraFrame, fg, Size(640,480));

		if(cameraFrame.empty())
		{
			std::cerr << "ERROR: NO CAMERA FRAME!?" << std::endl;
			exit(1);
		}
		HandShape hand;

		// HandShape can handle background subtraction if needed
		if(subtractBackground)
		{
			hand = HandShape(cameraFrame, background, learningRate);
		}
		else
		{
			hand = HandShape(cameraFrame);
		}

		Mat drawingContour, drawingHand;

		// Draw the hand if it's valid
		if(hand.isValidHand())
		{
			validHandCount = totalHandCount;
			hand.drawHand(drawingHand);

			hand.drawForegroundContours(drawingHand);
			flip(drawingHand, mirrorHand, 1);
			imshow("Hand", mirrorHand);
			writer << mirrorHand;
			detectGestureByMotion(hand);
			previousHand = hand;

		}
		else  // Otherwise just draw the camera frame
		{
			hand.drawForegroundContours(fg);
			flip(fg, mirror, 1);
			imshow("Hand", mirror);
			writer << mirror;
		}
		char keypress = waitKey(33);
		if(keypress == 27)
		{
			break;
		}
		else if(keypress == int('p'))
		{
			waitKey(0);
		}
	}
}

// Single image test requires a black and white image where the background is subtracted
int SingleImageTest(std::string filename)
{
	// Open image
	Mat src = imread(filename);
	if (src.empty())
	{
		std::cerr<<"Can't open file";
		return -1;
	}
	HandShape hand(src);
	Mat drawingContour, drawingHand;
	if(hand.isValidHand())
	{
		hand.drawContour(drawingContour);
		imshow("Contour", drawingContour);
		hand.drawHand(drawingHand);
		imshow("Hand", drawingHand);
		//detectGestureByMotion(hand);
	}
	imshow("Original", src);
	waitKey(0);
}

int main()
{
	CameraLoop();
	//CameraLoop("VideoDump.avi");
	//getchar();
	//SingleImageTest("TestVector\\im5.png");
}
