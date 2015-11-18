//maximum packet size is 100 bytes
#include "../sharedvars.h"
#include "../containers.h"

struct SensorData { 
	Vector3d gpsPos; 
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

StatusPayload pay; 
			
	SharedVars::flightModeLock.lock(); 
	pay.flight_mode = SharedVars::flightMode; 
	SharedVars::flightModeLock.unlock(); 
	SharedVars::batteryLevelLock.lock();
	pay.sensor.batteryLev = SharedVars::batteryLevel;
	SharedVars::batteryLevelLock.unlock();
	SharedVars::gyroDataLock.lock();
	pay.sensor.gyro_data = SharedVars::gyroData; 
	SharedVars::gyroDataLock.unlock();
	SharedVars::accDataLock.lock();
	pay.sensor.accel_data = SharedVars::accData; 
	SharedVars::accDataLock.unlock();
	SharedVars::magDataLock.lock();
	pay.sensor.mag_data = SharedVars::magData;
	SharedVars::magDataLock.unlock();
	SharedVars::currentGpsPositionLock.lock();
	pay.sensor.gpsPos = SharedVars::currentGpsPosition; 
	SharedVars::currentGpsPositionLock.unlock();
	
			//send_data(&pay, sizeof(pay), 's'); 
//return pay; 
} 

/*char * pointer_toStruct(Payload payload)
}*/ 
