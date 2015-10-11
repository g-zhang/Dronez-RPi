#include "AutoFlight.h"
#include "../containers.h"
#include "../sharedvars.h"

#include <iostream>
#include <cmath>
using namespace std;



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
}
