#ifndef RoadDetection_h
#define RoadDetection_h

#include "Camera.h"

#include <iostream>
#include <string>


using namespace std;

struct routeInfo{
	double distance;
	double heading;
};

vector<cv::Vec4i> findLines(cv::Mat src);

cv::Vec4i findAverageLine(vector<cv::Vec4i> lines);

double findRotationAmount(cv::Vec4i avgLine);

double findHorizontalShiftAmount(double height, cv::Vec4i line, double imageWidth);

cv::Mat takePic(raspicam::RaspiCam_Cv Camera);

void roadDetection_main();

#endif
