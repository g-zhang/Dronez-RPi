
//maximum packet size is 100 bytes
#include <sharedvars.h>
#include <containers.h>
#include <comm.h> 



struct CommandPayload { 
	FlightMode flight_mode; 
	Vector3d gpsPos; 

}; 


//static cast the payload into a struct and start interpreting, then send to UI or drone ( save it to shared vars)
//sendinig char array with character for type 




void interpretCommand_payload(char *data, int size){ 
	Payload pay; 

			//send_data(&pay, sizeof(pay), 'c'); 
			//receive the struct and determine whether or not the struct is the same 
			parse(data,'c'); 

			//maybe send over the last payload to see the size
				SharedVars::flightMode.lock(); 
				SharedVars::flightMode = pay.flight_mode; 
				SharedVars::flightMode.unlock(); 
				SharedVars::currentGpsPosition.lock(); 
				SharedVars::currentGpsPosition = pay.gpsPos; 
				SharedVars::currentGpsPosition.unlock();




			

}
