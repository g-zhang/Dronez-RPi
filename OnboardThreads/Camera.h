#ifndef Camera_h
#define Camera_h

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "math.h"
#include "../containers.h"
#include "../sharedvars.h"
#include <cstdlib>
#include "raspicam/raspicam_cv.h"

void initCamera();

cv::Mat takePic(raspicam::RaspiCam_Cv Camera);


unsigned char * matToBytes(Mat image);
#endif
