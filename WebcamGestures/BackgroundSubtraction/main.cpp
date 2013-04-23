#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>

using namespace cv;

int main(int argc, char *argv[])
{

	Mat frame;	// frame from camera
	Mat back;	// background frame
	Mat fore;	// foreground segment

	VideoCapture camera;
	int cameraNumber = 0;
	//camera.open("cam.mp4");

	// Open the camera
	camera.open(0);
	if(!camera.isOpened())
	{
		std::cerr << "ERROR: NO CAMERA AVAILABLE!?" << std::endl;
		return 1;
	}

	// Create an object of the background subtractor
	BackgroundSubtractorMOG2 background;
	
	// Contour points
	std::vector<std::vector<cv::Point> > contours;

	namedWindow("Frame");
	namedWindow("Background");

		
	while(1)
	{
		// Get frame
		camera >> frame;
		
		// Resize the frame
		resize(frame, frame, Size(640,480));
		
		// Feed subtractor new frame and get foreground
		background.operator ()(frame,fore);
		
		// Get the background. History is colleceted by background object
		background.getBackgroundImage(back);
		
		// Erode and Dilate to remove noise (seems to work)
		erode(fore,fore,cv::Mat());
		dilate(fore,fore,cv::Mat());
		
		// Find and draw Contours 
		findContours(fore,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
		drawContours(frame,contours,-1,cv::Scalar(0,0,255),2);
		
		// Show frame and saved background
		imshow("Frame", frame);
		imshow("Foreground", fore);
		imshow("Background", back);
		
		if(waitKey(30) == 27)
			break;
	}
	return 0;
}
