//sharedvars.h
#ifndef sharedvars_h
#define sharedvars_h

#include "containers.h"
#include "OnboardThreads/XbeeInterface.h"
#include <deque>
#include <queue>
#include <mutex>
#include <string>
#include <iostream>
#include <condition_variable>
#include "raspicam/raspicam_cv.h"

namespace SharedVars {
  extern raspicam::RaspiCam_Cv Camera;

  extern std::deque<Queue_send> infosend;
  extern std::mutex infosendLock;
  extern std::condition_variable infosendCv;

  extern double batteryLevel;
  extern std::mutex batteryLevelLock;

  extern Vector3d gyroData;
  extern std::mutex gyroDataLock;

  extern Vector3d accData;
  extern std::mutex accDataLock;

  extern Vector3d magData;
  extern std::mutex magDataLock;

  extern double barometerReading;
  extern std::mutex barometerReadingLock;

  extern double heading;
  extern std::mutex headingDataLock;

  extern FlightMode flightMode;
  extern std::mutex flightModeLock;

  extern std::queue<Vector3d> gpsFlightPlan;
  extern std::mutex gpsFlightPlanLock;

  extern Vector3d currentGpsPosition;
  extern std::mutex currentGpsPositionLock;
  extern Vector3d homeGpsPosition;

  extern double currentGpsSpeed;
  extern std::mutex currentGpsSpeedLock;

  extern double ultrasonicReading;
  extern std::mutex ultrasonicReadingLock;

  extern std::queue<void*> imageRecQueue;
  extern std::mutex imageRecQueueLock;

  extern int imageSize;
  extern std::mutex imageSizeLock;

  extern std::queue<void*> processedImages;
  extern std::mutex processedImagesLock;

  extern char* sendBuffer;
  extern std::mutex sendBufferLock;

  extern char* receiveBuffer;
  extern std::mutex receiveBufferLock;

  extern std::mutex printLock;
  //methods
  void init();
}

template <typename T>
void dprint(const T &msg) {
  SharedVars::printLock.lock();
  std::cout << msg << std::endl;
  SharedVars::printLock.unlock();
}

#endif
