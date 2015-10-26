#include <iostream>
#include <cmath>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>
#include <ctime>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <sys/select.h>

#include "AutoFlight.h"
#include "Utility.h"
#include "../containers.h"
#include "../sharedvars.h"
using namespace std;

//globals
char AQ32RecvBuffer[256]; //pre declared buffer for readAQ32Sensors
char AQ32SendBuffer[32];
int AQ32SendBufferLen = 0;
mutex AQFDLock;

int openAQ32Connection() {
  //setup connection
  int aqfd = -1;
  while(aqfd == -1) {
    aqfd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY);
    if(aqfd == -1) {
      dprint("Error could not open com port to AQ board\nRetrying in 10 secs . . .");
      this_thread::sleep_for(chrono::seconds(10));
    }
  }
  return aqfd;
}

int readfromAQ32(int aqfd, void* buf, size_t size) {
  int rxlen = read(aqfd, buf, size);
  if(rxlen < 0) {
    dprint("Error in read");
  }
  return rxlen;
}

int sendtoAQ32(int aqfd, void* buf, size_t size) {
  int err = -1;
  err = write(aqfd, buf, size);
  if(err < 0) {
    dprint("Error in sending bytes to AQ");
  } else if(err < int(size)) {
    dprint("Error: Not all bytes sent");
  }
  return err;
}

string readAQ32Sensors(int aqfd) {
  //ask AQ32 for data
  unsigned char c[3] = "w;";
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(aqfd, &fds);
  struct timeval timeout = { 0, AQ_TRANSFER_RATE };

  AQFDLock.lock();

  if(AQ32SendBufferLen > 0) {
    sendtoAQ32(aqfd, AQ32SendBuffer, AQ32SendBufferLen);
  } else {
    write(aqfd, &c, 2);
  }

  int ret = select(aqfd+1, &fds, NULL, NULL, &timeout);
  if(ret == 1) {
    dprint("begin read");
    unsigned num_vals = 0;
    unsigned size = 0;
    while(aqfd != -1 && num_vals < AQ_NUM_RVALS) {
      readfromAQ32(aqfd, AQ32RecvBuffer + size, 1);
      if(AQ32RecvBuffer[size] == ',') {
        num_vals += 1;
        //dprint(num_vals);
      }
      size += 1;
    }
    //read carriage return ln characters
    readfromAQ32(aqfd, AQ32RecvBuffer + size, 1);
    readfromAQ32(aqfd, AQ32RecvBuffer + size, 1);
    AQ32RecvBuffer[size] ='\0'; //null terminate string

    printf("size:%i  vals:%i\n", size, num_vals);
    printf("%s\n", AQ32RecvBuffer);
  } else {
    dprint("Error receving AQ response, timed out.");
  }
  AQFDLock.unlock();

  return ret == 1 ? string(AQ32RecvBuffer) : "";
}

void parseAQ32Sensors(string sensordata) {
  if(sensordata == "") {
    return;
  }

  vector<string> data = Util::split(sensordata, ',');

  SharedVars::batteryLevelLock.lock();
  SharedVars::batteryLevel = stod(data[BATTVOLT]);
  SharedVars::batteryLevelLock.unlock();

  SharedVars::gyroDataLock.lock();
  SharedVars::gyroData.x = stod(data[GYROX]);
  SharedVars::gyroData.y = stod(data[GYROY]);
  SharedVars::gyroData.z = stod(data[GYROZ]);
  SharedVars::gyroDataLock.unlock();

  SharedVars::accDataLock.lock();
  SharedVars::accData.x = stod(data[ACCELX]);
  SharedVars::accData.y = stod(data[ACCELY]);
  SharedVars::accData.z = stod(data[ACCELZ]);
  SharedVars::accDataLock.unlock();

  SharedVars::magDataLock.lock();
  SharedVars::magData.x = stod(data[MAGX]);
  SharedVars::magData.y = stod(data[MAGY]);
  SharedVars::magData.z = stod(data[MAGZ]);
  SharedVars::magDataLock.unlock();

  SharedVars::barometerReadingLock.lock();
  SharedVars::barometerReading = stod(data[BAROALT]);
  SharedVars::barometerReadingLock.unlock();

  SharedVars::headingDataLock.lock();
  SharedVars::heading = stod(data[HEADING]);
  SharedVars::headingDataLock.unlock();

  cout << SharedVars::accData.y << " " <<  SharedVars::accData.z << " " << SharedVars::heading << " " << SharedVars::barometerReading << endl;
}

void switchAQMode(int aqfd, int mode) {
  AQFDLock.lock();
  AQ32SendBuffer[0] = 'T';
  sprintf(AQ32SendBuffer + 1, "%d;w;", mode);
  AQ32SendBufferLen = 5;
  AQFDLock.unlock();
  //sendtoAQ32(aqfd, msg, 3, "send");
}

void AQSensorReaderMain(int aqfd) {
  dprint("Started AQ Sensor Reader Thread");
  //read loop
  while(1) {
    parseAQ32Sensors(readAQ32Sensors(aqfd));
    this_thread::sleep_for(chrono::microseconds(AQ_TRANSFER_RATE));
  }
}

double toRadians(const double &input) {
  return input * M_PI / 180;
}

//expects degrees, returrns radians
double findGPSHeading(const Vector3d& gpspt_a, const Vector3d& gpspt_b) {
  //assuming x is latitude, y is longitude
  const double& lat1 = toRadians(gpspt_a.x);
  const double& long1 = toRadians(gpspt_a.y);
  const double& lat2 = toRadians(gpspt_b.x);
  const double& long2 = toRadians(gpspt_b.y);

  double tmpy = sin(long2-long1) * cos(lat2);
  double tmpx = cos(lat1)*sin(lat2) - sin(lat1)*cos(lat2)*cos(long2-long1);

  return atan2(tmpy, tmpx);
}

double findGPSDistance(const Vector3d& gpspt_a, const Vector3d& gpspt_b) {
  const double& lat1 = toRadians(gpspt_a.x);
  const double& lat2 = toRadians(gpspt_b.x);
  const double& dlat = toRadians(gpspt_b.x - gpspt_a.x);
  const double& dlong = toRadians(gpspt_b.y - gpspt_a.y);

  double tmpa = pow(sin(dlat/2), 2) + cos(lat1)*cos(lat2) * pow(sin(dlong/2), 2);
  double tmpc = 2 * atan2(sqrt(tmpa), sqrt(1-tmpa));
  return EARTH_RADIUS * tmpc;
}

void autoflight_main() {
  dprint("Auto Flight Thread started");
  int aqfd = openAQ32Connection();
  std::thread AQSensorReaderThread(AQSensorReaderMain, aqfd);

  while(1) {
    // Vector3d a = {30, 50, 0};
    // Vector3d b = {60, 40, 0};
    // dprint(findGPSDistance(a, b));
    switchAQMode(aqfd, MANUAL_MODE);
    this_thread::sleep_for(chrono::seconds(5));
    switchAQMode(aqfd, AUTO_MODE);
    this_thread::sleep_for(chrono::seconds(5));
  }

}
