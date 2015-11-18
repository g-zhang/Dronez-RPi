//maximum packet size is 100 bytes
#include "../sharedvars.h"
#include "../containers.h"

struct Payload { 
	FlightMode flightMode; 
	Vector3d gpsPos; //idk
	double batteryLev; 
	Vector3d gyro_data; 
	Vector3d accel_data; 
	Vector3d mag_data; 
	double ultra_data; 


};
//get variables from UI and build the payload then send to max 


void build_payload(int packet_type){ 

Payload pay; 
	if (packet_type == 1) //command 
		{ 
			
			SharedVars::flightModeLock.lock(); 
			pay.flightMode = SharedVars::flightMode; 
			SharedVars::flightModeLock.unlock(); 
			SharedVars::currentGpsPositionLock.lock(); 
			pay.gpsPos = SharedVars::currentGpsPosition; 
			SharedVars::currentGpsPositionLock.unlock();

			send_data(&pay, sizeof(pay), 'c'); 
			

		}

	else if (packet_type == 2) //status 
		{ 
			
			SharedVars::flightModeLock.lock(); 
			pay.flightMode = SharedVars::flightMode; 
			SharedVars::flightModeLock.unlock(); 
			SharedVars::batteryLevelLock.lock();
			pay.batteryLev = SharedVars::batteryLevel;
			SharedVars::batteryLevelLock.unlock();
			SharedVars::gyroDataLock.lock();
			pay.gyro_data = SharedVars::gyroData; 
			SharedVars::gyroDataLock.unlock();
			SharedVars::accDataLock.lock();
			pay.accel_data = SharedVars::accData; 
			SharedVars::accDataLock.unlock();
			SharedVars::magDataLock.lock();
			pay.mag_data = SharedVars::magData;
			SharedVars::magDataLock.unlock();
			SharedVars::currentGpsPositionLock.lock();
			pay.gpsPos = SharedVars::currentGpsPosition; 
			SharedVars::currentGpsPositionLock.unlock();
			SharedVars::ultrasonicreadingLock.lock();
			pay.ultra_data = SharedVars::ultrasonicReading; 
			SharedVars::ultrasonicreadingLock.unlock();
			
			send_data(&pay, sizeof(pay), 's'); 

		}
	else 
		{ 
			cout << "Packet type not recognized\n"; 

		}


//return pay; 
} 

/*char * pointer_toStruct(Payload payload)
}*/ 
