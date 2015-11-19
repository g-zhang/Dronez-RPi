#include "RoadDetection.h"

using namespace std;
using namespace cv;
void imageQueueBuilder_main(){
	while(true){	
		Mat currentPic = takePic();

		//process Image



		//send Image
		char * imageByte = matToBytes(currentPic);
		unsigned int size = currentPic.total() * currentPic.elemSize();
		Queue_send toSend(matToBytes(currentPic), size, 'p') ;
		SharedVars::infosendLock.lock();
		SharedVars::infosend.push_back(toSend);
		SharedVars::infosendLock.unlock();
	}
}