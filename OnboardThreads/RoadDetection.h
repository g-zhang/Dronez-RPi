#ifndef RoadDetection_h
#define RoadDetection_h

#include "Camera.h"

#include <iostream>
#include <string>

#define ANGLE_OFFSET 0 //radians



using namespace std;

struct RouteInfo{
	double distance;
	double heading;
};

vector<cv::Vec4i> findLines(cv::Mat &src);

cv::Vec4i findAverageLine(vector<cv::Vec4i> &lines);

double findRotationAmount(cv::Vec4i avgLine);

double CalculatePixelsToMeters(double height, double imageWidth);

double findHorizontalShiftAmount(double height, cv::Vec4i line, double imageWidth);

RouteInfo findRouteInfo(cv::Vec4i avgLine, double pixelToMeters);

RouteInfo getNextRoadPoint();

#endif
