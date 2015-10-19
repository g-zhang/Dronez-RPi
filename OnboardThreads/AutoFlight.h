#ifndef AutoFlight_h
#define AutoFlight_h

#include "../containers.h"
#include "../sharedvars.h"

//constant defines
#define AQ_NUM_RVALS 20

enum AQDataValuesIndex {
  RPiMode = 0, RPiHeading, RPiAltitude, RPiPITCH, RPiTHROTTLE, BATTVOLT, BATTAMP, BATTUSED, GYROX, GYROY, GYROZ, ACCELX, ACCELY, ACCELZ, MAGX, MAGY, MAGZ, BAROALT, ALTHOLDSTATE, HEADING
};

double findGPSHeading(const Vector3d& gpspt_a, const Vector3d& gpspt_b);
void autoflight_main();

#endif
