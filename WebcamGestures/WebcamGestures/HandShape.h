#pragma once
#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <cmath>

using namespace cv;

class HandShape
{
public:
	HandShape(void);
	HandShape(vector<Point> startPoints, vector<Point> endPoints, vector<Point> defectPoints);
	~HandShape(void);
	void MakeHand(vector<Point> startPoints, vector<Point> endPoints, vector<Point> defectPoints);

	bool isValidHand();

	// Getters
	int getfingerCount();
	double getAngle();
	float getRadius();
	Point2f getCentroid();

	// Displayer
	void drawHand(Mat drawing);

private:
	// Members
	int fingerCount;
	Point2f middleFinger;	// heheh
	vector<Point> fingerPoints;
	bool isHand;

	double angle;
	Point2f centroid;
	float radius;
	double fingerThreshold;


	// Hull information
	vector<Point> startPoints;
	vector<Point> endPoints;
	vector<Point> defectPoints;

	// Methods
	void findCentroid(vector<Point> & points,Point2f & center,float & radius);
	double distance(Point2f one, Point2f two);
	void determineMiddleFinger();
	double angleBetween(Point2f origin, Point2f onCircle);
};