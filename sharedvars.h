//sharedvars.h
#ifndef sharedvars_h
#define sharedvars_h

#include "containers.h"
#include <queue>
#include <mutex>


namespace SharedVars {
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

  //methods
  void init();
}


#endif
