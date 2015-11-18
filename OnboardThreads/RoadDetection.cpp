#include "RoadDetection.h"

using namespace cv;

//This function will take an image as input and return all lines that are found
// in the image. The lines will be returned as a vector of Vec4i's. Vec4i is 
//structured as such [startX, startY, endX, endY]. If too many/too few lines
//are being detected the sensitivity of the algorithm can be adjusted by
//editing the parameters passed to HoughLinesP.
//For a description of each of the parameters see:
//http://docs.opencv.org/doc/tutorials/imgproc/imgtrans/hough_lines/hough_lines.html
vector<Vec4i> findLines(Mat src)
{

	vector<Vec4i> lines;
 	if(src.empty())
 	{
     		cout << "can not open " << endl;
		return lines;	
 	}	

 	Mat dst, cdst, smoothedImage;
	Canny(src, dst, 50, 200, 3);
	cvtColor(dst, cdst, CV_GRAY2BGR);
	GaussianBlur( dst, smoothedImage, Size( 31, 31 ), 0, 0 );
	HoughLinesP(dst, lines, 4, CV_PI/180, 100, 50, 10 );
	for( size_t i = 0; i < lines.size(); i++ )
	{
	 	Vec4i l = lines[i];
		cout << "Start" << "(" << l[0] << "," << l[1] << ")" << endl;	
		cout << "End" << "(" << l[2] << "," << l[3] << ")" << endl;
		line( smoothedImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
	}
	//imshow("source", src);
	//imshow("detected lines", smoothedImage);
	//waitKey();

	return lines;
}

//This function will look at all lines that have been detected and find the
//average line of all of them. Returned as a Vec4i.
Vec4i findAverageLine(vector<Vec4i> lines)
{
	
	int startX = 0;
	int startY = 0;
	int endX = 0;
	int endY = 0;
	for(unsigned int i = 0; i < lines.size(); ++i){
		Vec4i l = lines[i];
		startX += l[0];
		startY += l[1];
		endX += l[2];
		endY += l[3];	
	}
	startX = startX / lines.size();
	startY = startY / lines.size();
	endX = endX / lines.size();
	endY = endY / lines.size();
	
	return( Vec4i(startX, startY, endX, endY) );
}

//This function takes a line as input and returns the amount in degrees
//that that line must be rotated to be made vertical
double findRotationAmount(Vec4i avgLine)
{

	double deltaY = avgLine[3] - avgLine[1]; 
	double deltaX = avgLine[2] - avgLine[0];

	double rotateAmount = 0;
		
	if( deltaX == 0 )
		rotateAmount = 0;
	else{
		double angle = atan(deltaY / deltaX) * 180 / M_PI;
		
		if(angle <= 0)
			rotateAmount = 90 + angle;
		else{
			rotateAmount = 90 - angle;
			rotateAmount *= -1;
		}
	}

	return rotateAmount;
}

double CalculatePixelToMeters(double height, double imageWidth){
	double pictureWidth = height * tan(53.5);
	return pictureWidth / imageWidth;
}

double findHorizontalShiftAmount( double height, Vec4i line, double imageWidth){
	//Our Camera has a viewing angle of 53.5 degrees
	//This calculate the width of the picture in meters
	double pictureWidth = height * tan(53.5);

	double lineCenter = (line[0] + line[3]) / 2;

	double horizontalShift = (lineCenter/imageWidth) * pictureWidth;

	return horizontalShift; 

}

RouteInfo findLineEquation(Vec4i avgLine, double pixelToMeters){
	double slope = 0;
	double stepDistance = 5;

	slope = (avgLine[0] - avgLine[2]) / (avgLine[1] - avgLine[3]);
	double oldX = (avgLine[0] + avgLine[2]) / 2;
	double oldY = (avgLine[1] + avgLine[3]) / 2;
	
	double intercept = (avgLine[1] - (avgLine[0] * slope));

	double r = sqrt(1 + (slope * slope));
	
	double rot = findRotationAmount(avgLine);
	bool negativeSlope = false;
	if(rot < 0){
		negativeSlope = true;
		rot += 90;
	}
	double xMovement = stepDistance * cos(rot);
	double yMovement = stepDistance * sin(rot);

	double newX = 0;
	if(negativeSlope)
		newX = oldX - xMovement;
	else
		newX = oldX + xMovement;

	newY = oldY + yMovement;
	
	double distance = sqrt((newX * newX) + (newY * newY)) * pixelToMeters;

	double heading = atan2(newY, newX) * 180 / M_PI;

	RouteInfo routeInfo;
	routeInfo.distance = distance;
	routeInfo.heading = heading;

	return routeInfo;
}


void roadDetection_main(){	
	while(true){
		Mat src = takePic();
		vector<Vec4i> lines = findLines(src);
		if(lines.size() == 0){
			namedWindow("pic",CV_WINDOW_NORMAL);
			imshow("pic", src);
			resizeWindow("pic",800,400);
			waitKey();
			cout << "No lines found" << endl;
			continue;
		}
		
		int centerX = src.rows / 2;
		int centerY = src.cols / 2;
		Vec4i avgLine = findAverageLine(lines);

		cout << "Average Start: (" << avgLine[0] << "," << avgLine[1] << ")" << endl;
		cout << "Average End: (" << avgLine[2] << "," << avgLine[3] << ")" << endl;
		
		Vec4i newLine( avgLine[0]-centerX, centerY-avgLine[1], avgLine[2]-centerX,
				centerY-avgLine[3] );
		
		double rotateAmount = findRotationAmount(newLine);
		
		SharedVars::ultrasonicReadingLock.lock();
		double droneHeight = SharedVars::ultrasonicReading;
		SharedVars::ultrasonicReadingLock.unlock();	
		double horizontalShift = findHorizontalShiftAmount(droneHeight, newLine, src.rows);	

		cout << rotateAmount << endl;
		cout << horizontalShift << endl;


		line( src, Point(avgLine[0], avgLine[1]), Point(avgLine[2], avgLine[3]), Scalar(0,0,255), 3, CV_AA);
		namedWindow("avgLine",CV_WINDOW_NORMAL);
		imshow("avgLine", src);
		imwrite("avgLine.jpg", src);
		resizeWindow("avgLine",800,400);
		waitKey();
	}
}


