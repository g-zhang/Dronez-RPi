#include "StatusPayloadBuilder.h"


using namespace std;
void statusPayloadBuilder_main(){
	while(true){	
		StatusPayload payload = buildStatusPayload();

		Queue_send toSend(&payload, sizeof(StatusPayload), 's');
		SharedVars::infosendLock.lock();
		SharedVars::infosend.push_back(toSend);
		SharedVars::infosendLock.unlock();
		SharedVars::infosendCv.notify_one();	
		this_thread::sleep_for (chrono::seconds(1));	
	}
} 

StatusPayload buildStatusPayload(){
	StatusPayload toSend;

	SharedVars::flightModeLock.lock();
	toSend.flightMode = SharedVars::flightMode;
	SharedVars::flightModeLock.unlock();

	SharedVars::batteryLevelLock.lock();
	toSend.batteryLevel = SharedVars::batteryLevel;
	SharedVars::batteryLevelLock.unlock();

	SharedVars::gyroDataLock.lock();
	toSend.gyroData = SharedVars::gyroData;
	SharedVars::gyroDataLock.unlock();

	SharedVars::accDataLock.lock();
	toSend.accData = SharedVars::accData;
	SharedVars::accDataLock.unlock();

	SharedVars::magDataLock.lock();
	toSend.magData = SharedVars::magData;
	SharedVars::magDataLock.unlock();

	SharedVars::currentGpsPositionLock.lock();
	toSend.currentGPS = SharedVars::currentGpsPosition;
	SharedVars::currentGpsPositionLock.unlock();

	return toSend;
}
