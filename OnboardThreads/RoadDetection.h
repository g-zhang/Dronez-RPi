#ifndef RoadDetection_h
#define RoadDetection_h

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "math.h"
#include "../containers.h"
#include "../sharedvars.h"

#include <iostream>
#include <string>


using namespace std;

vector<cv::Vec4i> findLines(cv::Mat src);

cv::Vec4i findAverageLine(vector<cv::Vec4i> lines);

double findRotationAmount(cv::Vec4i avgLine);

double findHorizontalShiftAmount(double height, cv::Vec4i line, double imageWidth);

void roadDetection_main();

#endif
