//maximum packet size is 100 bytes
#include <sharedvars.h>
#include <containers.h>
#include <comm.h> 


struct Payload { 
	FlightMode flightMode; 
	int gpslist_size; //idk
	double batteryLevel; 
	Vector3d gyro-data; 
	Vector3d accel_data; 
	Vector3d mag_data; 
	double ultra_data; 

}; 


//static cast the payload into a struct and start interpreting, then send to UI or drone ( save it to shared vars)
//sendinig char array with character for type 




void interpret_payload(Payload * p2Payload, int packet_type){ 
	Payload pay; 
	if (packet_type == 1) //command 
		{  

			//send_data(&pay, sizeof(pay), 'c'); 
			//receive the struct and determine whether or not the struct is the same 
			parse(data,'c'); 

			if sizeof(pay) == sizeof(data){ //maybe send over the last payload to see the size
				SharedVars::flightMode.lock(); 
				SharedVars::flightMode = pay.flightMode; 
				SharedVars::flightMode.unlock(); 
				SharedVars::currentGpsPosition.lock(); 
				SharedVars::currentGpsPosition = pay.gpsPos; 
				SharedVars::currentGpsPosition.unlock();


			}


			

		}

	else if (packet_type == 2) //status 
		{ 
			parse(data,'s'); 
			
			SharedVars::flightMode.lock(); 
			SharedVars::flightMode = pay.flightMode; 
			SharedVars::flightMode.unlock(); 
			SharedVars::batteryLevel.lock();
			SharedVars::batteryLevel = pay.batteryLev;
			SharedVars::batteryLevel.unlock();
			SharedVars::gyroData.lock();
			SharedVars::gyroData = pay.gyro_data; 
			SharedVars::gyroData.unlock();
			SharedVars::accelData.lock();
			SharedVars::accelData = pay.accel_data; 
			SharedVars::accelData.unlock();
			SharedVars::magData.lock();
			SharedVars::magData = pay.mag_data;
			SharedVars::magData.unlock();
			SharedVars::currentGpsPosition.lock();
			SharedVars::currentGpsPosition = pay.gpsPos; 
			SharedVars::currentGpsPosition.unlock();
			SharedVars::ultrasonicreading.lock();
			SharedVars::ultrasonicReading = pay.ultra_data; 
			SharedVars::ultrasonicreading.unlock();
			
			


		}
	else 
		{ 
			cout << "Packet type not recognized\n"; 

		}
} 