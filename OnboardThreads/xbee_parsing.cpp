#include "xbee_parsing.h"
#include <string.h>
#include <iostream>
#include "../containers.h"
#include "../sharedvars.h"


using namespace std;
using namespace SharedVars;
void gps_parse(const char *data,const int size){
	cout<<size<<endl;
	Vector3d test;
	memcpy(&test, data, size);
	cout<<test.x<<" "<<test.y<<" "<<test.z<<endl;
	gpsFlightPlanLock.lock();
	SharedVars::gpsFlightPlan.push(test);
	gpsFlightPlanLock.unlock();
}

void flight_mode(const char *data,const int size){
	int temp;
	FlightMode test;
	memcpy(&temp, data, size);
	test = (FlightMode)temp;
	flightModeLock.lock();
	flightMode = test;
	flightModeLock.unlock();
}
