//sharedvars.h
#ifndef sharedvars_h
#define sharedvars_h

#include "containers.h"
#include <queue>
#include <mutex>


namespace SharedVars {
  //Shared Data
  extern int batteryLevel;
  extern Vector3d gyroData;
  extern Vector3d accData;
  extern double barometerReading;
  extern double magData;
  extern FlightMode flightMode;
  extern std::queue<Vector3d> gpsFlightPlan;
  extern Vector3d currentGpsPosition;
  extern double ultrasonicReading;
  extern std::queue<void*> imageRecQueue;
  extern int imageSize;
  extern std::queue<void*> processedImages;
  extern char* sendBuffer;
  extern char* receiveBuffer;

  //Variable locks
  extern std::mutex batteryLevelLock;
  extern std::mutex gyroDataLock;
  extern std::mutex accDataLock;
  extern std::mutex barometerReadingLock;
  extern std::mutex magDataLock;
  extern std::mutex flightModeLock;
  extern std::mutex gpsFlightPlanLock;
  extern std::mutex ultrasonicReadingLock;
  extern std::mutex imageRecQueueLock;
  extern std::mutex imageSizeLock;
  extern std::mutex processedImagesLock;
  extern std::mutex sendBufferLock;
  extern std::mutex receiveBufferLock;

  //getters and setters
  int getBatteryLevel();
}


#endif
