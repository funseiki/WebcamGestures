//#pragma once
#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <queue>

using namespace cv;

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

bool operator < (const QueuePoint & node1, const QueuePoint &node2)
{
	return node1.dist > node2.dist;
}
//Overload the > operator.
bool operator > (const QueuePoint & node1, const QueuePoint &node2)
{
	return node1.dist < node2.dist;
}