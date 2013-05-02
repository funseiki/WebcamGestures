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
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

#include "HandShape.h"
#include "HullMaker.h"
#include "Utils.h"

/** Namespaces **/
using namespace cv;

int BUFFER_SIZE = 6;
int MAX_FRAME_SIZE = 60;

std::vector<HandShape> hands;
int state = 0;//state = 0 implies SEARCHING and state = 1 implies RECOGNITION
HandShape startPoint;//stores the start point of our gesture search
HandShape endPoint;//stores the end point of our gesture search
int frameCount = 0;//stores number of frames we've seen during 'RECOGNTION' phase
int frameNumber = 0;//just for testing,can be removed later
Point2f averageStartCentroid;
Point2f averageEndCentroid;
HandShape previousHand;
int countStillFrame = 0;
void getGesture(){
		//Point2f startCentroid  = startPoint.getCentroid();
	   // Point2f endCentroid  = endPoint.getCentroid();
	std::ofstream myfile;
	myfile.open ("example.txt",std::ios::app);
	myfile << "StartCentroid x :" << averageStartCentroid.x <<" y: "<< averageStartCentroid.y << std::endl;
	myfile << "EndCentroid x :" << averageEndCentroid.x <<" y: "<< averageEndCentroid.y << std::endl;
	float angle = atan2(averageStartCentroid.y - averageEndCentroid.y, averageStartCentroid.x - averageEndCentroid.x);
		std::cout<<(angle*180/3.14);
		myfile << "Gesture finger count : " << startPoint.getfingerCount();
		myfile << "Gesture angle : " << angle;
		myfile << "Motion : "<< classifyMotion(angle,0.3,true,false) << std::endl;
		myfile.close();
}
bool isCurrentHandAMajority(HandShape handPair) {
	int frequencyOfShape = 0;
	int indexFirstOccurence = -1;
	for(int handsIterator = 0; handsIterator < hands.size(); handsIterator++) {
			HandShape hp = hands[handsIterator];
			if(hp.getfingerCount() == handPair.getfingerCount()){
				frequencyOfShape++;
				//store the first occurence of the hand match for assigning startPoint
				if(frequencyOfShape == 1)
					indexFirstOccurence = handsIterator;
				//When we find a majority in previous few frames,we assign start
				if(frequencyOfShape == (BUFFER_SIZE/2)){
					//if you are in the SEARCH phase,store the start point
					if(state == 0)
						startPoint = hands[indexFirstOccurence];
					return true;
				}
			}
		}
	if(state == 1)
		endPoint = hands[indexFirstOccurence];

	return false;

}

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

void detectGestureByMotion(HandShape currentHand){
	if(state == 0) { //Not started tracking 
		int dist = findDistanceBetween(currentHand.getCentroid(),previousHand.getCentroid());
		if(dist < 10) {
			countStillFrame++;
			if(countStillFrame == 5){
				setAverageCentroid();
				state = 1;		
				std::cout <<"Tracking started"<<std::endl;
			}
		} else {
			countStillFrame = 0;
		}
	}
	hands.push_back(currentHand);
	if(hands.size() > BUFFER_SIZE) hands.erase(hands.begin());
}
void detectGesture(HandShape currentHandPair){
	std::ofstream myfile;
	myfile.open ("example.txt",std::ios::app);
	//Searching for a start point to start gesture recognition
	 
	if(state == 0) {
	//	std::cout << "Searching phase";
	//	myfile << "Searching phase" << std::endl;
		bool foundStart = isCurrentHandAMajority(currentHandPair);
		if(foundStart) {
			setAverageCentroid();
			state = 1;
		}

	} else { //RECOGNITION PHASE
		frameCount++;
		//std::cout << "Recognition phase";
		//myfile << "Recognition phase" << std::endl;
		bool foundEnd =  !isCurrentHandAMajority(startPoint);
		if(foundEnd || (frameCount == MAX_FRAME_SIZE)){
			setAverageCentroid();
			getGesture();
			frameCount = 0;
			state = 0;
		}

	}
	hands.push_back(currentHandPair);
	if(hands.size() > BUFFER_SIZE) hands.erase(hands.begin());
	myfile.close();
}

void CameraLoop(std::string filename = "")
{

	// 2.0 Api
	VideoCapture camera;

	// Open the camera
	//camera.open(0);
	if(filename.length() > 0)
	{
		camera.open(filename);
	}
	else
	{
		camera.open(0);
	}
	if(!camera.isOpened())
	{
		std::cerr << "ERROR: NO CAMERA AVAILABLE!?" << std::endl;
		return;
	}

	// Set camera parameters
	//camera.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	//camera.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	int totalHandCount = 0;
	int validHandCount = 0;
	
	while(true)
	{
		totalHandCount++;
		if((totalHandCount - validHandCount) > 10 && state == 1){
			setAverageCentroid();
			getGesture();
			state = 0;
			std::cout <<"Tracking stopped"<<std::endl;
			previousHand.~HandShape();
			new(&previousHand) HandShape();
		}
		Mat cameraFrame;

		// Gets the current camera frame
		camera >> cameraFrame;

		if(cameraFrame.empty())
		{
			std::cerr << "ERROR: NO CAMERA FRAME!?" << std::endl;
			exit(1);
		}

		HandShape hand(cameraFrame);
		Mat drawingContour, drawingHand;
		if(hand.isValidHand())
		{
			validHandCount = totalHandCount;
			std::cout << "Frame No :" << totalHandCount;
			hand.drawHand(drawingHand);
			imshow("Hand", drawingHand);
			std::cout <<" Current centroid - x : "<< hand.getCentroid().x <<" y: :" <<  hand.getCentroid().y;
			std::cout << "Distance :" << findDistanceBetween(hand.getCentroid(),previousHand.getCentroid()) << std::endl;
			waitKey(0);
		//	if(count > 30)
			detectGestureByMotion(hand);
			previousHand = hand;
		}

		// Display the interesting thing
		imshow("Cam", cameraFrame);
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
		detectGesture(hand);
	}
	imshow("Original", src);
	waitKey(0);
}

int main()
{
	CameraLoop("VideoDump.avi");
	//getchar();
	//SingleImageTest("TestVector\\im2.png");
}
