#ifndef AutoFlight_h
#define AutoFlight_h

#include "../containers.h"
#include "../sharedvars.h"

double findGPSHeading(const Vector3d& gpspt_a, const Vector3d& gpspt_b);
void autoflight_main();

#endif
