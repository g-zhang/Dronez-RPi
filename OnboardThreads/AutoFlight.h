#ifndef AutoFlight_h
#define AutoFlight_h

#include <mutex>

#include "../containers.h"
#include "../sharedvars.h"

/*** CONSTANT DEFINES ***/
#define MICROSECOND 1000000.0 //number of microsecs in a sec

#define AQ_TRANSFER_RATE 300000 //microsecs between each query
#define AQ_TIMEOUT_COUNT 3 //number of timeouts before re-opening the port
#define AQ_MAX_RETRY_TIME 10 //seconds for re-opening the serial connection
#define AQ_READBUF_SIZE 256
#define AQ_SENDBUF_SIZE 128

#define EARTH_RADIUS 6371000 //m

/** FLIGHT CONFIG VALUES **/
#define GPS_DIST 3 //m, the error margin of distance in meters considered "at" a gps point
#define GPS_HEADING_ERROR_MARGIN 0.1 //radians, error margin in radians to be considered "on target"

#define DEFAULT_YAW_VALUE 1500 //integer, PWD amount for no change in yaw
#define TURN_YAW_OFFSET 50 //integer, PWD amount to change in yaw

#define DEFAULT_PITCH_VALUE 1500 //integer, PWD amount to keep craft stable
#define TURN_PITCH_VALUE 1550 //integer, PWD amount to pitch the craft forward

#define CV_HOLD_TIME 3 //#of loops to hold at gps point before taking picture and calculating next route

/*** ENUM AND TYPES ***/

enum AQDataValuesIndex {
  AQMode = 0, BATTVOLT, BATTAMP, BATTUSED, ACCELX, ACCELY, ACCELZ, BAROALT, HEADING, AQ_NUM_RVALS
};

enum AQCmdValuesIndex {
  RPiPITCH = 0, RPiROLL, RPiHeading, RPiAltitude
};

enum AQMode {
  MANUAL_MODE = 0, AUTO_MODE, AUTO_MODE2
};


/*** METHODS ***/
void readGPSValues();
double findRelativeHeading(const double &heading, const double &target);
double constrainRadians(const double &input, const double &min, const double &max);
double findGPSHeading(const Vector3d& gpspt_a, const Vector3d& gpspt_b);
double findGPSDistance(const Vector3d& gpspt_a, const Vector3d& gpspt_b);
Vector3d findGPSPoint(const Vector3d& gpspt_a, const double& heading, const double& distance);
void autoflight_main();

#endif
