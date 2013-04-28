#pragma once
#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>
using namespace cv;

class HandShape
{
public:
	HandShape(void);
	HandShape(vector<Point> startPoints, vector<Point> endPoints, vector<Point> defectPoints);
	~HandShape(void);
	void MakeHand(vector<Point> startPoints, vector<Point> endPoints, vector<Point> defectPoints);

	// Getters
	int getfingerCount();
	int getAngle();
	float getRadius();
	Point2f getCentroid();

	// Displayer
	void drawHand(Mat drawing);
private:
	// Members
	int fingerCount;
	int angle;
	Point2f centroid;
	float radius;
	double fingerThreshold;

	// Hull information
	vector<Point> startPoints;
	vector<Point> endPoints;
	vector<Point> defectPoints;

	// Methods
	void findCentroid(vector<Point> & points,Point2f & center,float & radius);
	double distance(Point one, Point two);
};