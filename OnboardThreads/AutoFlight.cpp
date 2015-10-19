#include <iostream>
#include <cmath>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>
#include <sstream>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART

#include "AutoFlight.h"
#include "Utility.h"
#include "../containers.h"
#include "../sharedvars.h"
using namespace std;

//globals
char AQ32RecvBuffer[256]; //pre declared buffer for readAQ32Sensors

int openAQ32Connection() {
  int aqfd = -1;
  aqfd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY);
  if(aqfd == -1) {
    cout << "Error could not open com port to AQ board" << endl;
  }
  return aqfd;
}

int readBytes(int aqfd, char* buf, size_t size) {
  int rxlen = read(aqfd, (void*)buf, size);
  if(rxlen < 0) {
    cout << "Error in read" << endl;
  }
  return rxlen;
}

string readAQ32Sensors(int aqfd) {
  //ask AQ32 for data
  unsigned char c = 'w';
  if(write(aqfd, &c, sizeof(unsigned char)) < 0) {
    cout << "Error in sending bytes" << endl;
  }

  unsigned num_vals = 0;
  unsigned size = 0;
  while(aqfd != -1 && num_vals < AQ_NUM_RVALS) {
    readBytes(aqfd, AQ32RecvBuffer + size, 1);
    if(AQ32RecvBuffer[size] == ',') {
      num_vals += 1;
    }
    size += 1;
  }
  //read carriage return ln characters
  readBytes(aqfd, AQ32RecvBuffer + size, 1);
  readBytes(aqfd, AQ32RecvBuffer + size, 1);
  AQ32RecvBuffer[size] ='\0'; //null terminate string

  printf("size:%i  vals:%i\n", size, num_vals);
  printf("%s\n", AQ32RecvBuffer);

  return string(AQ32RecvBuffer);
}

void parseAQ32Sensors(string sensordata) {
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

void autoflight_main() {
  cout << "Auto Flight Thread started" << endl;
  int aqfd = openAQ32Connection();
  while(1) {
    parseAQ32Sensors(readAQ32Sensors(aqfd));
    this_thread::sleep_for(chrono::seconds(2));
  }
}
