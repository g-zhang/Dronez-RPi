//maximum packet size is 100 bytes
#include "sharedvars.h"
#include "containers.h"

struct Payload { 
	FlightMode flight_mode; 
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
			
			SharedVars::flightMode.lock(); 
			pay.flightMode = SharedVars::flightMode; 
			SharedVars::flightMode.unlock(); 
			SharedVars::currentGpsPosition.lock(); 
			pay.gpsPos = SharedVars::currentGpsPosition; 
			SharedVars::currentGpsPosition.unlock();

			send_data(&pay, sizeof(pay), 'c'); 
			

		}

	else if (packet_type == 2) //status 
		{ 
			
			SharedVars::flightMode.lock(); 
			pay.flightMode = SharedVars::flightMode; 
			SharedVars::flightMode.unlock(); 
			SharedVars::batteryLevel.lock();
			pay.batteryLev = SharedVars::batteryLevel;
			SharedVars::batteryLevel.unlock();
			SharedVars::gyroData.lock();
			pay.gyro_data = SharedVars::gyroData; 
			SharedVars::gyroData.unlock();
			SharedVars::accelData.lock();
			pay.accel_data = SharedVars::accelData; 
			SharedVars::accelData.unlock();
			SharedVars::magData.lock();
			pay.mag_data = SharedVars::magData;
			SharedVars::magData.unlock();
			SharedVars::currentGpsPosition.lock();
			pay.gpsPos = SharedVars::currentGpsPosition; 
			SharedVars::currentGpsPosition.unlock();
			SharedVars::ultrasonicreading.lock();
			pay.ultra_data = SharedVars::ultrasonicReading; 
			SharedVars::ultrasonicreading.unlock();
			
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