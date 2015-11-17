#include "parsing.h"
#include "containers.h"
#include <string.h>
#include <iostream>


using namespace std;

void command_parse(const char *data,const int size){
	FlightMode flight_mode;
	Vector3d gpsPos; 
	int size = 0; 
	memcpy(&flight_mode, data, sizeof(flight_mode));
	size += sizeof(flight_mode); 
	memcpy(&gpsPos, data + size, sizeof(gpsPos)); 
}

void status_parse(const char *data,const int size){
	FlightMode flight_mode;
	Vector3d gpsPos; 
	double batteryLev; 
	Vector3d gyro_data; 
	Vector3d accel_data; 
	Vector3d mag_data; 
	double ultra_data; 
	int size = 0; 

	memcpy(&flight_mode, data, sizeof(flight_mode));
	size += sizeof(flight_mode); 
	memcpy(&gpsPos, data + size, sizeof(gpsPos)); 
	size += sizeof(gpsPos); 
	memcpy(&batteryLev, data + size , sizeof(batteryLev));
	size += sizeof(batteryLev); 
	memcpy(&gyro_data, data + size, sizeof(gyro_data)); 
	size += sizeof(gyro_data); 
	memcpy(&accel_data, data + size, sizeof(accel_data));
	size += sizeof(accel_data); 
	memcpy(&mag_data, data + size, sizeof(mag_data)); 
	size += sizeof(mag_data); 
	memcpy(&ultra_data, data + size, sizeof(ultra_data)); 
	size += sizeof(ultra_data); 
}

