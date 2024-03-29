#include <iostream>
#include <iomanip>
#include <cmath>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>
#include <cstring>
#include <time.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <queue>
#include <sys/select.h>

#include "AutoFlight.h"
#include "Utility.h"
#include "gps.h"
#include "RoadDetection.h"
#include "../containers.h"
#include "../sharedvars.h"
using namespace std;

//globals
char AQ32RecvBuffer[AQ_READBUF_SIZE]; //pre declared buffer for readAQ32Sensors
char AQ32SendBuffer[AQ_SENDBUF_SIZE];
int AQ32SendBufferLen = 0;
mutex AQFDLock;
Vector3d prevGPSLocation = { 0, 0, 0 };

//cv mode globals
int cvModeHoldCount = 0;
queue<Vector3d> cvFlightPlan;
mutex cvFlightPlanLock;

int openAQ32Connection() {
  //setup connection
  dprint("Opening serial port to AQ32");
  int aqfd = -1;
  unsigned retrytime = 1;
  while(aqfd == -1) {
    aqfd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY);
    if(aqfd == -1) {
      dprint("Error could not open com port to AQ board\nRetrying . . .");
      this_thread::sleep_for(chrono::seconds(retrytime));
      //increase back off time
      retrytime = retrytime + 1 < AQ_MAX_RETRY_TIME ? retrytime + 1 : AQ_MAX_RETRY_TIME;
    }
  }

  // set baud rate
  struct termios tty;
  tcgetattr(aqfd, &tty);
  cfsetispeed (&tty, (speed_t)B115200);
  cfsetospeed (&tty, (speed_t)B115200);
  tty.c_cc[VMIN]   =  1;                  // read doesn't block
  tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
  cfmakeraw(&tty);
  tcflush(aqfd, TCIOFLUSH);
  if ( tcsetattr (aqfd, TCSANOW, &tty) != 0) {
     std::cout << "Error " << errno << " from tcsetattr" << std::endl;
  }

  //zero out buffers
  memset(AQ32RecvBuffer, '\0', AQ_READBUF_SIZE);
  memset(AQ32SendBuffer, '\0', AQ_SENDBUF_SIZE);

  return aqfd;
}

int readfromAQ32(int aqfd, void* buf, size_t size, timeval* timeout) {
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(aqfd, &fds);
  int ret = select(aqfd+1, &fds, NULL, NULL, timeout);
  if (ret == 1) {
    int rxlen = read(aqfd, buf, size);
    if(rxlen < 0) {
      dprint("readfromAQ32: Error in read");
    }
    return rxlen;
  } else if (ret == 0) {
    dprint("readfromAQ32: AQ Read timed out");
  } else {
    dprint("readfromAQ32: Error in read");
  }
  return ret;
}

int sendtoAQ32(int aqfd, void* buf, size_t size) {
  int err = -1;
  err = write(aqfd, buf, size);
  if(err < 0) {
    dprint("sendtoAQ32: Error in sending bytes to AQ");
  } else if(err < int(size)) {
    dprint("sendtoAQ32: Error, not all bytes sent");
  }
  return err;
}

//returns a string of sensor reading, returns emptry string on error
string performAQ32IO(int aqfd) {
  unsigned char c[3] = "w;";
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(aqfd, &fds);
  struct timeval timeout = { 0, AQ_TRANSFER_RATE };
  struct timeval timeout2 = { 0, 100 };

  tcflush(aqfd, TCIOFLUSH);
  memset(AQ32RecvBuffer, '\0', AQ_READBUF_SIZE);
  if(sendtoAQ32(aqfd, &c, 2) < 2) {
    return "";
  }

  int ret = select(aqfd+1, &fds, NULL, NULL, &timeout);
  if(ret == 1) {
    unsigned num_vals = 0;
    unsigned size = 0;
    while(aqfd != -1 && num_vals < AQ_NUM_RVALS) {
      int val = readfromAQ32(aqfd, AQ32RecvBuffer + size, 1, &timeout2);
      if(val <= 0) {
        return "";
      }
      if(AQ32RecvBuffer[size] == ',') {
        num_vals += 1;
        //dprint(num_vals);
      }
      size += 1;
    }
    //read carriage return ln characters
    if (readfromAQ32(aqfd, AQ32RecvBuffer + size, 1, &timeout2) <= 0) { return ""; }
    if (readfromAQ32(aqfd, AQ32RecvBuffer + size, 1, &timeout2) <= 0) { return ""; }
    AQ32RecvBuffer[size] ='\0'; //null terminate string

    SharedVars::printLock.lock();
    // printf("size:%i  vals:%i\n", size, num_vals);
    // printf("%s\n", AQ32RecvBuffer);
    cout << AQ32RecvBuffer[0] << endl;
    SharedVars::printLock.unlock();
  }

  return ret == 1 ? string(AQ32RecvBuffer) : "";
}

bool parseAQ32Sensors(string sensordata) {
  if(sensordata == "") {
    return false;
  }
  vector<string> data = Util::split(sensordata, ',');

  SharedVars::batteryLevelLock.lock();
  SharedVars::batteryLevel = stod(data[BATTVOLT]);
  SharedVars::batteryLevelLock.unlock();

  SharedVars::accDataLock.lock();
  SharedVars::accData.x = stod(data[ACCELX]);
  SharedVars::accData.y = stod(data[ACCELY]);
  SharedVars::accData.z = stod(data[ACCELZ]);
  SharedVars::accDataLock.unlock();

  SharedVars::barometerReadingLock.lock();
  SharedVars::barometerReading = stod(data[BAROALT]);
  SharedVars::barometerReadingLock.unlock();

  SharedVars::headingDataLock.lock();
  SharedVars::heading = stod(data[HEADING]);
  SharedVars::headingDataLock.unlock();

  //cout << SharedVars::accData.y << " " <<  SharedVars::accData.z << " " << SharedVars::heading << " " << SharedVars::barometerReading << endl;
  return true;
}

void createAQFlightCommand(int mode, int RPiPITCH, int RPiYAW, double RPiAltitude) {
  SharedVars::printLock.lock();
  cout << mode << " " << RPiPITCH << " " << RPiYAW << " " << RPiAltitude << endl;
  SharedVars::printLock.unlock();
  AQFDLock.lock();
  memset(AQ32SendBuffer, '\0', AQ_SENDBUF_SIZE);
  AQ32SendBuffer[0] = 'S';
  sprintf(AQ32SendBuffer + 1, "%d;%d;%d;%.2f;", mode, RPiPITCH, RPiYAW, RPiAltitude);
  AQ32SendBufferLen = strlen(AQ32SendBuffer);
  AQFDLock.unlock();
}

void sendAQFlightCommand(int aqfd) {
    AQFDLock.lock();
    tcflush(aqfd, TCIOFLUSH);
    sendtoAQ32(aqfd, AQ32SendBuffer, AQ32SendBufferLen);
    memset(AQ32SendBuffer, '\0', AQ_SENDBUF_SIZE);
    AQ32SendBufferLen = 0;
    AQFDLock.unlock();
}

void AQDebugMode() {
  //turn back and forth every ~3 sec
  static long counter = 0;
  static int turnval = TURN_YAW_OFFSET;
  if(counter % 10 == 0) {
    turnval *= -1;
  }
  createAQFlightCommand(AUTO_MODE, DEFAULT_PITCH_VALUE, DEFAULT_YAW_VALUE + turnval, 0);
  dprint(DEFAULT_YAW_VALUE + turnval);
  counter++;
}

void AQDebugMode2() {
  static long counter2 = 0;
  static int pitchval = DEFAULT_PITCH_VALUE;
  static int offset = -10;
  if(counter2 % 10 == 0) {
    offset *= -1;
  }
  pitchval += offset;
  createAQFlightCommand(AUTO_MODE, pitchval, DEFAULT_YAW_VALUE, 0);
  dprint(pitchval);
  counter2++;
}

void AQLand() {
    SharedVars::barometerReadingLock.lock();
    dprint("AQ: LANDING MODE");
    if(SharedVars::barometerReading > LANDING_THRESHOLD) {
      dprint("Landing...");
      createAQFlightCommand(AUTO_MODE, DEFAULT_PITCH_VALUE, DEFAULT_YAW_VALUE, -ALTITUDE_BUMP_SPEED);
    } else {
      SharedVars::flightMode = MANUAL;
    }
    SharedVars::barometerReadingLock.unlock();
}

void AQGoHome() {
    SharedVars::gpsFlightPlanLock.lock();
    dprint("AQ: HOME command received, deleting gps flight plan");
    while(!SharedVars::gpsFlightPlan.empty()) {
      SharedVars::gpsFlightPlan.pop();
    }
    dprint("Adding gps home point to flightplan");
    SharedVars::gpsFlightPlan.push(SharedVars::homeGpsPosition);

    SharedVars::flightMode = GPS;
    SharedVars::gpsFlightPlanLock.unlock();
}

void AQGPSFlight() {
  //gps mode
  SharedVars::gpsFlightPlanLock.lock();
  SharedVars::headingDataLock.lock();
  SharedVars::barometerReadingLock.lock();

  if(SharedVars::gpsFlightPlan.empty()) { //queue is empty, don't do anything
    dprint("GPS flight plan queue is empty!");
    createAQFlightCommand(MANUAL_MODE, DEFAULT_PITCH_VALUE, DEFAULT_YAW_VALUE, 0);

  } else { //otherwise perform auto gps flight

    double distancetoTarget = findGPSDistance(SharedVars::currentGpsPosition, SharedVars::gpsFlightPlan.front());
    double headingtoTarget = findGPSHeading(SharedVars::currentGpsPosition, SharedVars::gpsFlightPlan.front());
    double angleDeltaToTarget = findRelativeHeading(SharedVars::heading, headingtoTarget);
    SharedVars::printLock.lock();
    cout << "current lat/long: " << fixed << setprecision(6) << SharedVars::currentGpsPosition.x << " " << SharedVars::currentGpsPosition.y << endl;
    cout << "target lat/long: " << fixed << setprecision(6) <<  SharedVars::gpsFlightPlan.front().x << " " << SharedVars::gpsFlightPlan.front().y << endl;
    cout << "distancetoTarget: " << distancetoTarget << endl;
    cout << "headingtoTarget: " << headingtoTarget << endl;
    cout << "angleDeltaToTarget: " << angleDeltaToTarget << endl;
    SharedVars::printLock.unlock();

    if(distancetoTarget <= GPS_DIST) { //see if we have arrived within 3 meters of our target
      dprint("GPS MODE: Arrived at a GPS point");
      //remove the the waypoint from the queue
      SharedVars::gpsFlightPlan.pop();
      //reset all flight vals and have the drone keep over point until next loop
      createAQFlightCommand(MANUAL_MODE, DEFAULT_PITCH_VALUE, DEFAULT_YAW_VALUE, 0);

    } else { //otherwise send flight values
      //correct heading
      if(abs(angleDeltaToTarget) > GPS_HEADING_ERROR_MARGIN) {
        dprint("Correcting Heading");
        //if our heading is not correct, turn by DEFAULT_YAW_VALUE radians
        int commandyaw = DEFAULT_YAW_VALUE;
        if(angleDeltaToTarget < 0) {
          commandyaw -= TURN_YAW_OFFSET;
        } else if(angleDeltaToTarget > 0) {
          commandyaw += TURN_YAW_OFFSET;
        }
        createAQFlightCommand(AUTO_MODE, DEFAULT_PITCH_VALUE, commandyaw, 0);

      //corect speed/pitch
      } else {
        dprint("Correcting Pitch");
        //calculate estimated position until next loop
        //double estimatedDistance = (AQ_TRANSFER_RATE / MICROSECOND) * SharedVars::currentGpsSpeed;
        //adjust pitch angle to move craft forwarding
        createAQFlightCommand(AUTO_MODE, TURN_PITCH_VALUE, DEFAULT_YAW_VALUE, 0);
      }
    }
  }

  SharedVars::barometerReadingLock.unlock();
  SharedVars::headingDataLock.unlock();
  SharedVars::gpsFlightPlanLock.unlock();
}

void AQCVFlight() {
  //cv mode
  cvFlightPlanLock.lock();
  SharedVars::headingDataLock.lock();
  SharedVars::barometerReadingLock.lock();

  if(cvFlightPlan.empty()) { //queue is empty, read to take picture
    dprint("CV Mode Taking Picture");
    //reset hold count
    cvModeHoldCount = 0;
    //call distance and heading
    RouteInfo route = getNextRoadPoint();

    SharedVars::printLock.lock();
    cout << "AFlight:Roadline Info >> Distance = " << route.distance << ", RelHeading = ";
    cout << fixed << setprecision(9) << route.heading << endl;
    SharedVars::printLock.unlock();

    route.heading = constrainRadians(route.heading + SharedVars::heading, -M_PI, M_PI);

    SharedVars::printLock.lock();
    cout << "AFlight:Roadline Info >> True Heading = " <<  route.heading << endl;
    SharedVars::printLock.unlock();

    cvFlightPlan.push(findGPSPoint(SharedVars::currentGpsPosition, route.heading, route.distance));
    createAQFlightCommand(MANUAL_MODE, DEFAULT_PITCH_VALUE, DEFAULT_YAW_VALUE, 0);

  } else { //otherwise perform auto gps flight

    double distancetoTarget = findGPSDistance(SharedVars::currentGpsPosition, cvFlightPlan.front());
    double headingtoTarget = findGPSHeading(SharedVars::currentGpsPosition, cvFlightPlan.front());
    double angleDeltaToTarget = findRelativeHeading(SharedVars::heading, headingtoTarget);
    SharedVars::printLock.lock();
    cout << "current lat/long: " << fixed << setprecision(6) << SharedVars::currentGpsPosition.x << " " << SharedVars::currentGpsPosition.y << endl;
    cout << "target lat/long: " << fixed << setprecision(6) << cvFlightPlan.front().x << " " << cvFlightPlan.front().y << endl;
    cout << "distancetoTarget: " << distancetoTarget << endl;
    cout << "headingtoTarget: " << headingtoTarget << endl;
    cout << "angleDeltaToTarget: " << angleDeltaToTarget << endl;
    SharedVars::printLock.unlock();

    if(distancetoTarget <= CV_GPS_DIST) {
      dprint("CV MODE: Arrived at a GPS point, Holding position");
      cvModeHoldCount += 1;
    }

    if(cvModeHoldCount >= CV_HOLD_TIME) { //see if we have arrived within 3 meters of our target
      dprint("CV MODE: Removing waypoint");
      //remove the the waypoint from the queue
      cvFlightPlan.pop();
      //reset all flight vals and have the drone keep over point until next loop
      createAQFlightCommand(MANUAL_MODE, DEFAULT_PITCH_VALUE, DEFAULT_YAW_VALUE, 0);

    } else { //otherwise send flight values
      //correct heading
      if(abs(angleDeltaToTarget) > GPS_HEADING_ERROR_MARGIN) {
        dprint("Correcting Heading");
        //if our heading is not correct, turn by DEFAULT_YAW_VALUE radians
        int commandyaw = DEFAULT_YAW_VALUE;
        if(angleDeltaToTarget < 0) {
          commandyaw -= TURN_YAW_OFFSET;
        } else if(angleDeltaToTarget > 0) {
          commandyaw += TURN_YAW_OFFSET;
        }
        createAQFlightCommand(AUTO_MODE, DEFAULT_PITCH_VALUE, commandyaw, 0);

      //corect speed/pitch
      } else {
        dprint("Correcting Pitch");
        //calculate estimated position until next loop
        //double estimatedDistance = (AQ_TRANSFER_RATE / MICROSECOND) * SharedVars::currentGpsSpeed;
        //adjust pitch angle to move craft forwarding
        createAQFlightCommand(AUTO_MODE, TURN_PITCH_VALUE, DEFAULT_YAW_VALUE, 0);
      }
    }
  }

  SharedVars::barometerReadingLock.unlock();
  SharedVars::headingDataLock.unlock();
  cvFlightPlanLock.unlock();
}

void AQManualMode() {
  dprint("AQ: Manual Mode");

  cvFlightPlanLock.lock();
  while(!cvFlightPlan.empty()) {
    cvFlightPlan.pop();
  }
  cvFlightPlanLock.unlock();

  SharedVars::gpsFlightPlanLock.lock();
  while(!SharedVars::gpsFlightPlan.empty()) {
    SharedVars::gpsFlightPlan.pop();
  }
  SharedVars::gpsFlightPlanLock.unlock();

  createAQFlightCommand(MANUAL_MODE, DEFAULT_PITCH_VALUE, DEFAULT_YAW_VALUE, 0);
}

void AQFlightLogic(int aqfd) {
  SharedVars::flightModeLock.lock();

  #ifdef ENABLE_BATTERY_AUTO
    SharedVars::batteryLevelLock.lock();
    if(SharedVars::batteryLevel <= MIN_BATT_VOLT) {
      if(SharedVars::flightMode != LAND) {
        dprint("Warning: Critical Battery. Switching to autoland mode");
        SharedVars::flightMode = LAND;
      }
    } else if(SharedVars::batteryLevel <= MIN_BATT_VOLT_HOME) {
      if(SharedVars::flightMode != HOME) {
        dprint("Warning: Low Battery. Switching to home GPS mode");
        SharedVars::flightMode = HOME;
      }
    }
    SharedVars::batteryLevelLock.unlock();
  #endif

  SharedVars::currentGpsPositionLock.lock();
  SharedVars::currentGpsSpeedLock.lock();
  readGPSValues();

  if(SharedVars::flightMode == GPS) {
    AQGPSFlight();
  }
  else if(SharedVars::flightMode == CVMODE) {
    AQCVFlight();
  }
  else if(SharedVars::flightMode == HOME) {
    AQGoHome();
  }
  else if(SharedVars::flightMode == LAND) {
    AQLand();
  }
  else { //manual mode
    AQManualMode();
  }
  sendAQFlightCommand(aqfd);

  SharedVars::currentGpsSpeedLock.unlock();
  SharedVars::currentGpsPositionLock.unlock();
  SharedVars::flightModeLock.unlock();
}

void AQBoardIOMain() {
  dprint("Started AQ I/O Thread");
  int aqfd = openAQ32Connection();
  int timeoutcount = 0;
  //main loop
  while(1) {
    clock_t t = clock();

    if(aqfd == -1) {
      dprint("ERROR: LOST CONNECTION TO AQ32!!!");
      aqfd = openAQ32Connection();
    }
    if(parseAQ32Sensors(performAQ32IO(aqfd))) {
      //reset timeout count after every successful I/O event
      timeoutcount = 0;
      //successful reading of AQ board state, calculate movements and send to AQ
      AQFlightLogic(aqfd);
    } else {
      //TIMED OUT BEHAVIOR
      dprint("ERROR: TIMEOUT 1");
      //flush the AQ buffer
      tcflush(aqfd, TCIOFLUSH);

      //timeout logic
      timeoutcount++;
      if(timeoutcount >= AQ_TIMEOUT_COUNT) {
        dprint("ERROR: Too many consecutive timeouts!!!\nReopening connection");
        close(aqfd);
        aqfd = openAQ32Connection();
        dprint("Done! Successfully Reconnected.");
        timeoutcount = 0;
      }
    }

    t = clock() - t;
    double secs = ((float)t) / CLOCKS_PER_SEC;
    long usecs = round(secs * MICROSECOND);
    usecs = AQ_TRANSFER_RATE - usecs;
    usecs = usecs > 0 ? usecs : 0;
    cout << "took: " << secs << " | sleeping for " << usecs << endl;
    usleep(usecs);
  }
}

double toRadians(const double &input) {
  return input * M_PI / 180;
}

double toDegrees(const double &input) {
	return input * 180 / M_PI;
}

double constrainRadians(const double &input, const double &min, const double &max) {
  double ret = input;
  while(ret < min) {
    ret += (2 * M_PI);
  }
  while(ret >= max) {
    ret -= (2 * M_PI);
  }
  return ret;
}

double findRelativeHeading(const double &heading, const double &target) {
  double relativeHeading = constrainRadians(target, -M_PI, M_PI) - constrainRadians(heading, -M_PI, M_PI);
  return constrainRadians(relativeHeading, -M_PI, M_PI);
}

//expects degrees, returns radians between 0 and 2PI
double findGPSHeading(const Vector3d& gpspt_a, const Vector3d& gpspt_b) {
  //assuming x is latitude, y is longitude
  const double& lat1 = toRadians(gpspt_a.x);
  const double& long1 = toRadians(gpspt_a.y);
  const double& lat2 = toRadians(gpspt_b.x);
  const double& long2 = toRadians(gpspt_b.y);

  double tmpy = sin(long2-long1) * cos(lat2);
  double tmpx = cos(lat1)*sin(lat2) - sin(lat1)*cos(lat2)*cos(long2-long1);

  return constrainRadians(atan2(tmpy, tmpx), 0, M_PI * 2);
}

//returns in meters
double findGPSDistance(const Vector3d& gpspt_a, const Vector3d& gpspt_b) {
  const double& lat1 = toRadians(gpspt_a.x);
  const double& lat2 = toRadians(gpspt_b.x);
  const double& dlat = toRadians(gpspt_b.x - gpspt_a.x);
  const double& dlong = toRadians(gpspt_b.y - gpspt_a.y);

  double tmpa = pow(sin(dlat/2), 2) + cos(lat1)*cos(lat2) * pow(sin(dlong/2), 2);
  double tmpc = 2 * atan2(sqrt(tmpa), sqrt(1-tmpa));
  return EARTH_RADIUS * tmpc;
}

//distance in meters
//returns in lat long degrees
Vector3d findGPSPoint(const Vector3d& gpspt_a, const double& heading, const double& distance) {
	Vector3d target;
	const double& lat1 = toRadians(gpspt_a.x);
	const double& long1 = toRadians(gpspt_a.y);
	double angularDistance = distance / EARTH_RADIUS;
	target.x = asin(sin(lat1)*cos(angularDistance) + cos(lat1)*sin(angularDistance)*cos(heading));
	target.y = long1 + atan2(sin(heading)*sin(angularDistance)*cos(lat1),
                           cos(angularDistance) - sin(lat1)*sin(target.x));
	target.x = toDegrees(target.x);
	target.y = toDegrees(target.y);
	return target;
}

void readGPSValues() {
  loc_t data;
  gps_location(&data);
  prevGPSLocation = SharedVars::currentGpsPosition;
  SharedVars::currentGpsPosition.x = data.latitude;
  SharedVars::currentGpsPosition.y = data.longitude;
  SharedVars::currentGpsPosition.z = data.altitude;
  SharedVars::currentGpsSpeed = data.speed;

  SharedVars::printLock.lock();
  cout << "GPS: Moved " << findGPSDistance(prevGPSLocation, SharedVars::currentGpsPosition);
  cout << " meters, Speed: " << SharedVars::currentGpsSpeed << endl;
  SharedVars::printLock.unlock();
}

void autoflight_main() {
  //init home gps point
  dprint("Initializing home GPS point");
  loc_t data;
  gps_location(&data);
  SharedVars::homeGpsPosition.x = data.latitude;
  SharedVars::homeGpsPosition.y = data.longitude;

  dprint("Auto Flight Thread started");
  AQBoardIOMain();
}
