#include "Utils.h"

using namespace std;

Point2f findCluster(vector <Point> pointData, vector <int> &clusterPoints, int clusterCount =3 )
{
	int sampleCount = pointData.size();
	int dimensions = 2;
	float * pointsdata = new float[sampleCount*2]; //[] = {1,1, 2,2, 6,6, 5,5, 10,10};

	int cnt = 0;
	for(int a=0; a<sampleCount; a++){
	pointsdata[cnt] = pointData[a].x;
	cnt++;
	pointsdata[cnt] = pointData[a].y;
	cnt++;
	}
	Mat points(sampleCount,dimensions, CV_32F,pointsdata);

	//int clusterCount = 3; //i want 3 averaged points back

	cv::Mat labels;
	Mat centers(clusterCount, 1, points.type());

	kmeans(points, clusterCount, labels, cv::TermCriteria(), 2,cv::KMEANS_PP_CENTERS, centers);

	//std::cout<<"Data: \n"<<points<<std::endl;
	//std::cout<<"Labels: \n"<<labels<<std::endl;

	int labelPt0 = 0;//labels.at<int>(0,1);
	int labelPt1 = 0;
	int labelPt2 = 0;

	for(int i=0; i<sampleCount; i++)
	{
		int val = labels.at<int>(i);
		if(val == 0 )
			labelPt0++;
		else if(val == 1)
			labelPt1++;
		else if (val==2)
			labelPt2++;
	}

	//std::priority_queue <labelStruct> PQ;

	std::priority_queue <labelStruct, vector<labelStruct>, std::greater<vector<labelStruct>::value_type> > PQ;

	PQ.push(labelStruct(labelPt0,0));
	PQ.push(labelStruct(labelPt1,1));
	PQ.push(labelStruct(labelPt2,2));
	//std::cout<<"\n\nSize of PQ: "<<PQ.size();

	int topCluster0 = PQ.top().label;
	int topClusterSize0 = PQ.top().labelCount;
	PQ.pop();
	int topCluster1 = PQ.top().label;
	int topClusterSize1 = PQ.top().labelCount;
	PQ.pop();
	int topCluster2 = PQ.top().label;
	int topClusterSize2 = PQ.top().labelCount;
	PQ.pop();

	float cX = centers.at<float>(topCluster0,0);
	float cY = centers.at<float>(topCluster0,1);
	Point2f foundCentroid(cX,cY);

	//std::cout<<labels;
	//    std::cout<<"\n\nLabel0: "<<clusterChoice0<<"\n\nLabel1: "<<clusterChoice1<<"\n\nLabel2: "<<clusterChoice2;
	if(topClusterSize2 == topClusterSize1)
	{
		std::priority_queue <QueuePoint2> findLabels;


		for(int i=0; i<clusterCount; i++)
		{
			float X = centers.at<float>(i,0);
			float Y = centers.at<float>(i,1);

			Point2f clusterCenter(X,Y);

			QueuePoint2 qpl (clusterCenter, foundCentroid, i );

			findLabels.push(qpl);
		}

		int topLabel0 = findLabels.top().label;
		findLabels.pop();
		int topLabel1 = findLabels.top().label;

		//std::cout<<"\nCluster Labels: "<<topLabel0<<" "<<topLabel1;

		for(int i=0; i<sampleCount; i++)
		{
			int val = labels.at<int>(i);

			if((val == topLabel0) || (val == topLabel1))
				clusterPoints.push_back(i);
		}

	}
	else
	{
		for(int i=0; i<sampleCount; i++)
		{
			int val = labels.at<int>(i);

			if((val == topCluster0) || (val == topCluster1))
				clusterPoints.push_back(i);
		}
	}
	//std::cout<<"clustered points: "<<clusterPoints.size();
	delete[] pointsdata;
	return foundCentroid;
	//return centers;
}

int classifyMotion(float Orientation, float thresholdRange, bool isRadians = true, bool showRange = false)
{
	float pi;

	if(isRadians)
		pi = PI;
	else
		pi = 180;

	float Range = thresholdRange * pi/2;
	float vectorOrientation = Orientation;

	//Horizontal is at 0 or 2pi rad
	//Right is at 0 or 2*pi rad
	//Up is at pi/2 rad
	//Left is  pi rad
	//Down is at 3pi/4

	float U = (pi/2);
	float D = (3*pi/2);
	float L = (pi);
	float Ru = (0.00);
	float Rd = (2*pi);

	float Up = (pi/2) + Range;
	float Un = (pi/2) - Range;
	float Dp = (3*pi/2) + Range;
	float Dn = (3*pi/2) - Range;
	float Lp = pi + Range;
	float Ln = pi - Range;
	float Rp = 0 + Range;
	float Rn = (2*pi) - Range;

	bool isNotInRange = false;

	bool isUp = false;
	bool isDown = false;
	bool isLeft = false;
	bool isRight = false;

	if(showRange)
	{
		cout<<"\nUP Range: "<<Up<<" - "<<Un;
		cout<<"\nDOWN Range: "<<Dp<<" - "<<Dn;
		cout<<"\nLEFT Range: "<<Lp<<" - "<<Ln;
		cout<<"\nRIGHT Range: "<<Rp<<" - "<<Rn;
		cout<<"\nRange: "<<Range;
		cout<<"\nInput:"<<vectorOrientation;
	}

	if(((vectorOrientation<= Up)&&(vectorOrientation >= U)) || ((vectorOrientation<= U)&&(vectorOrientation >= Un)) )
	{
		isUp = true;
		//cout<<"\nUp!";
	}

	if(((vectorOrientation<= Dp)&&(vectorOrientation >= D)) || ((vectorOrientation<= D)&&(vectorOrientation >= Dn)) )
	{
		isDown = true;
		//cout<<"\nDown!";
	}

	if(((vectorOrientation<= Lp)&&(vectorOrientation >= L)) || ((vectorOrientation<= L)&&(vectorOrientation >= Ln)) )
	{
		isLeft = true;
		//cout<<"\nLeft";
	}

	if(((vectorOrientation<= Rp)&&(vectorOrientation >= Ru)) || ((vectorOrientation<= Rd)&&(vectorOrientation >= Rn)) )
	{
		isRight = true;
		//cout<<"\nRight";
	}



	if(isRight || isLeft || isUp || isDown)
		isNotInRange = false;
	else
		isNotInRange = true;

	if(isNotInRange)
	{
		cerr<<"\nNot in Range!\n";

		float distRp = vectorOrientation - Rp;
		float distRn = Rn - vectorOrientation;

		float distLp = vectorOrientation - Lp;
		float distLn = Ln - vectorOrientation;

		float distUp = vectorOrientation - Up;
		float distUn = Un - vectorOrientation;

		float distDp = vectorOrientation - Dp;
		float distDn = Dn - vectorOrientation;

		float distR = (distRn > distRp) ? distRp : distRn;
		float distL = (distLn > distLp) ? distLp : distLn;
		float distD = (distDn > distDp) ? distDp : distDn;
		float distU = (distUn > distUp) ? distUp : distUn;

		float dist;

		if (distU >= distD && distU >= distR)
				dist = UP;
		else if (distD >=distU && distD >=distR)
				dist = DOWN;
		else if (distR >= distU && distR >=distD)
				dist = RIGHT;

		if(dist == UP)
			return ((distU > distL) ? LEFT : UP );

		else if (dist == DOWN)
			return ((distD > distL) ? LEFT : DOWN );

		else if (dist == RIGHT)
			return ((distR > distL) ? LEFT : RIGHT );

		return -1;
	}

	if(isUp)
		return UP;
	if(isDown)
		return DOWN;
	if(isLeft)
		return LEFT;
	if (isRight)
		return RIGHT;
}

double findDistanceBetween(Point2f one, Point2f two) {
	return sqrt(((one.x - two.x)*(one.x - two.x))+((one.y - two.y)*(one.y - two.y)));
}


//returns in radians
double findAngleBetween(Point2f origin, Point2f onCircle)
{
	float pi = 3.14159;

	float diffY = onCircle.y - origin.y;
	float diffX = onCircle.x - origin.x;
	float slope = diffY/diffX;

	//Find angle of line
	double angle =  atan(slope);

	//Convert to catesian co-ordinates

	if ((origin.x >= onCircle.x) && (origin.y <= onCircle.y))
	{
		angle = (pi) - angle;
	}

	else if ((origin.x <= onCircle.x) && (origin.y <= onCircle.y))
	{
		angle = (2*pi) - angle;
	}

	else if ((origin.x <= onCircle.x) && (origin.y >= onCircle.y))
	{
		angle = 0.00 - angle;
	}

	else if ((origin.x >= onCircle.x) && (origin.y >= onCircle.y))
	{
		angle = pi - angle;
	}


	return angle;
}

bool operator < (const QueuePoint & node1, const QueuePoint &node2)
{
	return node1.dist > node2.dist;
}
//Overload the > operator.
bool operator > (const QueuePoint & node1, const QueuePoint &node2)
{
	return node1.dist < node2.dist;
}


bool operator < (const QueuePoint2 & node1, const QueuePoint2 &node2)
{
	return node1.dist > node2.dist;
}
//Overload the > operator.
bool operator > (const QueuePoint2 & node1, const QueuePoint2 &node2)
{
	return node1.dist < node2.dist;
}

bool operator < (const labelStruct & node1, const labelStruct &node2)
{
	return node1.labelCount > node2.labelCount;
}
//Overload the > operator.
bool operator > (const labelStruct & node1, const labelStruct &node2)
{
	return node1.labelCount < node2.labelCount;
}
