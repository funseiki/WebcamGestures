#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>

using namespace cv;

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


int main(int argc, char *argv[])
{

	Mat frame;    // frame from camera
	Mat back;    // background frame
	Mat fore;    // foreground segment
	Mat yuv;
	Mat drawing;

	VideoCapture camera;
    int cameraNumber = 0;
    //camera.open("cam.mp4");
    camera.open(0);
	VideoWriter writerFore("VideoDumpForeAsh1.avi",CV_FOURCC('P', 'I', 'M', '1'), 20, Size(640,480), false);
	VideoWriter writerCam("VideoDumpCamAsh1.avi",CV_FOURCC('P', 'I', 'M', '1'), 20, Size(640,480), false);


    int history = 500;
    float learningRate = 0.0001;

    // Create an object of the background subtractor
	BackgroundSubtractorMOG2 background(history,16,true);

    // Contour points
	std::vector<std::vector<cv::Point> > contours;

	namedWindow("Frame");
	namedWindow("Background");


	while(1)
	{
   	 // Get frame
    	camera >> frame;
		writerCam<<drawing;
   	 // Resize the frame
    	resize(frame, frame, Size(640,480));

    	cvtColor(frame, yuv, CV_BGR2YCrCb);

   	 // Feed subtractor new frame and get foreground
   	 background.operator ()(yuv,fore,learningRate);

   	 // Get the background. History is colleceted by background object
    	background.getBackgroundImage(back);

   	 // Erode and Dilate to remove noise (seems to work)
    	erode(fore,fore,cv::Mat());
    	dilate(fore,fore,cv::Mat());

   	 // Find and draw Contours
    	findContours(fore,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    	drawContours(frame,contours,-1,cv::Scalar(0,0,255),2);

    	int s = findBiggestContour(contours);

    // Draw and fill detector contour
    	drawing = Mat::zeros( frame.size(), CV_8UC1 );
    	drawContours( drawing, contours, s, Scalar(255), -1, 8);//, hierarchy, 0, Point() );

		writerFore<<drawing;


   	 // Show frame and saved background
   	 imshow("Frame", frame);
   	 imshow("Foreground", fore);
    	imshow("Background", back);
    	imshow("Drawing", drawing);

   	 if(waitKey(30) == 27)
   	 {
   	 	break;

   	 }

	}

	imwrite("Segment.jpg",drawing);
	return 0;
}