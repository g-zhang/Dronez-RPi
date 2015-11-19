#include "sharedvars.h"

#include <iostream>
using namespace std;

namespace SharedVars {
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
  std::cout << "Initializing shared vars" << endl;
}
