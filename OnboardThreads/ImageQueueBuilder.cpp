#include "RoadDetection.h"

using namespace std;

void imageQueueBuilder_main(){
	while(true){	
		cv::Mat currentPic = takePic();

		//process Image

		//send Image
		unsigned int size = currentPic.total() * currentPic.elemSize();
		Queue_send toSend(matToBytes(currentPic), size, 'p') ;
		SharedVars::infosendLock.lock();
		SharedVars::infosend.push_back(toSend);
		SharedVars::infosendLock.unlock();
	}
}
