#ifndef AutoFlight_h
#define AutoFlight_h

#include <mutex>

#include "../containers.h"
#include "../sharedvars.h"

/*** CONSTANT DEFINES ***/
#define AQ_NUM_RVALS 20
#define AQ_TRANSFER_RATE 300000 //microsecs
#define EARTH_RADIUS 6371 //km

/*** ENUM AND TYPES ***/

enum AQDataValuesIndex {
  AQMode = 0, AQHeading, AQAltitude, AQPITCH, AQROLL, BATTVOLT, BATTAMP, BATTUSED, GYROX, GYROY, GYROZ, ACCELX, ACCELY, ACCELZ, MAGX, MAGY, MAGZ, BAROALT, ALTHOLDSTATE, HEADING
};

enum AQCmdValuesIndex {
  RPiPITCH = 0, RPiROLL, RPiHeading, RPiAltitude
};

enum AQMode {
  MANUAL_MODE = 0, AUTO_MODE, AUTO_MODE2
};


/*** METHODS ***/
double findGPSHeading(const Vector3d& gpspt_a, const Vector3d& gpspt_b);
void autoflight_main();

#endif
