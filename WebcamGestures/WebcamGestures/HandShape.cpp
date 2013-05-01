#include "HandShape.h"
#include <limits>
#include "QueuePoint.cpp"
using namespace std;

HandShape::HandShape(void)
{
	isHand = false;
	fingerCount = -1;
	angle = -1;
	centroid = Point(0,0);
	middleFinger = Point2f(-1,-1);
	fingerThreshold = 15;
}

HandShape::HandShape(vector<Point> _startPoints, vector<Point> _endPoints, vector<Point> _defectPoints)
{
	fingerThreshold = 15;
	MakeHand(_startPoints, _endPoints, _defectPoints);

}

HandShape::HandShape(int fingerCount,Point2f centroid){
	this->fingerCount = fingerCount;
	this->centroid = centroid;
}

HandShape::~HandShape(void)
{
}

bool HandShape::isValidHand()
{
	return isHand;
}

void HandShape::determineMiddleFinger()
{
	float rad;

	// The 'middle finger' is the centroid of all of the other fingers
	findCentroid(fingerPoints, middleFinger, rad);
}

void HandShape::getClosestPoints(Point2f center, vector<Point> points, vector<Point> & out, int threshold=5)
{
	priority_queue<QueuePoint> pq;

	// Order by closest to the center
	for(int i = 0; i < points.size(); i++)
	{
		pq.push(QueuePoint(points[i], center));
	}

	if(pq.size() < threshold)
	{
		threshold = pq.size();
	}

	int count = 0;
	while(count < threshold)
	{
		out.push_back(pq.top().Pt);
		pq.pop();
		count++;
	}
}

void HandShape::determineHandCenter(vector<Point> points)
{
	Point2f center;
	vector<Point> closest;
	float rad;
	findCentroid(points, center, rad);
	getClosestPoints(center, points, closest, 3);
	findCentroid(closest, center, rad);
	centroid = center;
	radius = rad;
}

// Helpers
void HandShape::MakeHand(vector<Point> _startPoints, vector<Point> _endPoints, vector<Point> _defectPoints)
{
	if(!_startPoints.size() || !_endPoints.size() || !_defectPoints.size())
	{
		isHand = false;
		return;
	}

	startPoints = _startPoints;
	endPoints = _endPoints;
	defectPoints = _defectPoints;
	fingerCount = 0;

	// Find the centroid of this hand
	//findCentroid(defectPoints, centroid, radius);
	determineHandCenter(defectPoints);

	// Count the fingers
	Point prevEnd = endPoints[endPoints.size()-1];
	for (int i = 0; i < startPoints.size(); i++)
	{
		Point currStart = startPoints[i];
		double curr = distance(currStart, prevEnd);

		// The start and end points are close enough to be a finger
		if(curr < fingerThreshold)
		{
			fingerCount++;
			fingerPoints.push_back(currStart);
		}
		prevEnd = endPoints[i];
	}

	// Figure out the orientation of the hand
	determineMiddleFinger();
	angle = angleBetween(centroid, middleFinger);

}

// Utility functions
double HandShape::angleBetween(Point2f origin, Point2f onCircle)
{
	return (double)atan((double)((onCircle.y - origin.y)/ (onCircle.x - origin.x)));
}

double HandShape::distance(Point2f one, Point2f two)
{
	return sqrt(((one.x - two.x)*(one.x - two.x))+((one.y - two.y)*(one.y - two.y)));
}

void HandShape::findCentroid(vector<Point> & points,Point2f & center,float & radius)
{
	if(points.size() > 0)
	{
		minEnclosingCircle(points,center, radius);
	}
	else
	{
		center.x = -1;
		center.y = -1;
		radius = -1;
	}
}

// Displayers
void HandShape::drawHand(Mat drawing)
{
	for(int i = 0; i<startPoints.size(); i++)
	{
		// Draw a yellow line from start to defect
		line(drawing, startPoints[i], defectPoints[i], Scalar(0,255,255), 1);

		// Draw start point (Green)
		circle(drawing, startPoints[i], 10, Scalar(0,250,0));

		// Draw end point (White)
		circle(drawing, endPoints[i], 10, Scalar(250,250,250));

		// Draw defect point (Red)
		circle(drawing, defectPoints[i], 5, Scalar(0,0,255));
	}

	if(centroid.x > 0 && centroid.y > 0 && radius > 0)
	{
		circle(drawing, centroid, radius, Scalar(255,0,255));
		circle(drawing, centroid, 10, Scalar(255, 255, 0));
	}

	imshow("Defects", drawing);
}

// Getters
int HandShape::getfingerCount()
{
	return fingerCount;
}
double HandShape::getAngle()
{
	return angle;
}
float HandShape::getRadius()
{
	return radius;
}
Point2f HandShape::getCentroid()
{
	return centroid;
}
Point2f HandShape::getMiddleFinger()
{
	return middleFinger;
}