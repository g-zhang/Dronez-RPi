#include "sharedvars.h"

#include <iostream>
using namespace std;

namespace SharedVars {
  int batteryLevel;
  Vector3d gyroData;
  Vector3d accData;
  double barometerReading;
  double magData;
  FlightMode flightMode;
  std::queue<Vector3d> gpsFlightPlan;
  Vector3d currentGpsPosition;
  double ultrasonicReading;
  std::queue<void*> imageRecQueue;
  int imageSize;
  std::queue<void*> processedImages;
  char* sendBuffer;
  char* receiveBuffer;

  //Variable locks
  std::mutex batteryLevelLock;
  std::mutex gyroDataLock;
  std::mutex accDataLock;
  std::mutex barometerReadingLock;
  std::mutex magDataLock;
  std::mutex flightModeLock;
  std::mutex gpsFlightPlanLock;
  std::mutex ultrasonicReadingLock;
  std::mutex imageRecQueueLock;
  std::mutex imageSizeLock;
  std::mutex processedImagesLock;
  std::mutex sendBufferLock;
  std::mutex receiveBufferLock;
}

void SharedVars::init() {
  std::cout << "Initializing shared vars" << endl;

}

int SharedVars::getBatteryLevel() {
  batteryLevelLock.lock();
  int ret = batteryLevel;
  batteryLevelLock.unlock();
  return ret;
}
