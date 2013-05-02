#pragma once
#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define PI 3.14159265

int classifyMotion(float Orientation, float thresholdRange, bool isRadians, bool showRange);
double findAngleBetween(Point2f origin, Point2f onCircle);

// Priority Queue Comparer
class QueuePoint
{
public:
	Point2f Pt;
	Point2f Centroid;
	double dist;

	friend bool operator < (const QueuePoint & node1, const QueuePoint &node2);
	friend bool operator > (const QueuePoint & node1, const QueuePoint &node2);

	QueuePoint(Point2f P, Point2f C)
	{
		Pt = P;
		Centroid = C;
		dist = sqrt(((P.x - C.x)*(P.x - C.x))+((P.y - C.y)*(P.y - C.y)));
	}
};