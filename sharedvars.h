//sharedvars.h
#include "containers.h"
#include <queue>
#include <mutex>

using namespace std;

#ifndef sharedvars_h
#define sharedvars_h

//Shared Data
int batteryLevel;
Coord3d gyroData;
Coord3d accData;
double barometerReading;
double magData;
FlightMode flightMode;
queue<Coord3d> gpsFlightPlan;
Coord3d currentGpsPosition;
double ultrasonicReading;
queue<void*> imageRecQueue;
int imageSize;
queue<void*> processedImages;
char* sendBuffer;
char* receiveBuffer;

//Variable locks
mutex batteryLevelLock;
mutex gyroDataLock;
mutex accDataLock;
mutex barometerReadingLock;
mutex magDataLock;
mutex flightModeLock;
mutex gpsFlightPlanLock;
mutex ultrasonicReadingLock;
mutex imageRecQueueLock;
mutex imageSizeLock;
mutex processedImagesLock;
mutex sendBufferLock;
mutex receiveBufferLock;

#endif
