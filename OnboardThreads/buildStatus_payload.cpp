//maximum packet size is 100 bytes
#include "sharedvars.h"
#include "containers.h"

struct SensorData { 

	Vector3D gpsPos; 
	Vector3d accel_data; 
	Vector3d gyro_data; 
	Vector3d mag_data; 
	int batteryLev; 

};

struct StatusPayload { 
	FlightMode flight_mode; 
	SensorData sensor; 

};
//get variables from UI and build the payload then send to max 


void buildStatus_payload(char* data, int size){ 

Payload pay; 
			
	SharedVars::flightMode.lock(); 
	pay.flight_mode = SharedVars::flightMode; 
	SharedVars::flightMode.unlock(); 
	SharedVars::batteryLevel.lock();
	pay.sensor.batteryLev = SharedVars::batteryLevel;
	SharedVars::batteryLevel.unlock();
	SharedVars::gyroData.lock();
	pay.sensor.gyro_data = SharedVars::gyroData; 
	SharedVars::gyroData.unlock();
	SharedVars::accelData.lock();
	pay.sensor.accel_data = SharedVars::accelData; 
	SharedVars::accelData.unlock();
	SharedVars::magData.lock();
	pay.sensor.mag_data = SharedVars::magData;
	SharedVars::magData.unlock();
	SharedVars::currentGpsPosition.lock();
	pay.sensor.gpsPos = SharedVars::currentGpsPosition; 
	SharedVars::currentGpsPosition.unlock();
	SharedVars::ultrasonicreading.lock();
	pay.sensor.ultra_data = SharedVars::ultrasonicReading; 
	SharedVars::ultrasonicreading.unlock();
	
			//send_data(&pay, sizeof(pay), 's'); 
//return pay; 
} 

/*char * pointer_toStruct(Payload payload)
}*/ 