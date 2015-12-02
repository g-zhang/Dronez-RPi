#include "sharedvars.h"
#include "OnboardThreads/Camera.h"
#include "OnboardThreads/gps.h"
#include <iostream>
using namespace std;

namespace SharedVars {
  //raspicam::RaspiCam_Cv Camera;
  std::mutex cameraLock;

  std::deque<Queue_send> infosend;
  std::mutex infosendLock;
  std::condition_variable infosendCv;

  std::mutex printLock;

  double batteryLevel;
  std::mutex batteryLevelLock;

  Vector3d gyroData;
  std::mutex gyroDataLock;

  Vector3d accData;
  std::mutex accDataLock;

  Vector3d magData;
  std::mutex magDataLock;

  double barometerReading;
  std::mutex barometerReadingLock;

  double heading;
  std::mutex headingDataLock;

  FlightMode flightMode;
  std::mutex flightModeLock;

  std::queue<Vector3d> gpsFlightPlan;
  std::mutex gpsFlightPlanLock;

  Vector3d currentGpsPosition;
  std::mutex currentGpsPositionLock;
  Vector3d homeGpsPosition;

  double currentGpsSpeed;
  std::mutex currentGpsSpeedLock;

  double ultrasonicReading;
  std::mutex ultrasonicReadingLock;

  std::queue<void*> imageRecQueue;
  std::mutex imageRecQueueLock;

  int imageSize;
  std::mutex imageSizeLock;

  std::queue<void*> processedImages;
  std::mutex processedImagesLock;

  char* sendBuffer;
  std::mutex sendBufferLock;

  char* receiveBuffer;
  std::mutex receiveBufferLock;


}

void SharedVars::init() {
  cout << "Initializing shared vars" << endl;
  //init flightmode
  flightMode = MANUAL;
  //init home gps point
  //cout << "Initializing home GPS point" << endl;
  //loc_t* data = GPS_info();
  //homeGpsPosition.x = data->latitude;
  //homeGpsPosition.y = data->longitude;
  //init camera
  initCamera();
}
