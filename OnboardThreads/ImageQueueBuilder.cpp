#include "RoadDetection.h"
#include <unistd.h>

using namespace std;
using namespace cv;

void imageQueueBuilder_main(){
	int i = 0;
	Mat currentPic;
	Mat smallerPic;
	Mat bw;
	while(true){
		cout << "Taking vid feed img" << endl;	
		currentPic = takePic();
		Size picSize;
		picSize.height = 192;
		picSize.width = 340;
		resize(currentPic, smallerPic, picSize);
		cvtColor(smallerPic, bw, CV_RGB2GRAY);
		vector<int> compression_params;
    		compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    		compression_params.push_back(35);
		string name = "compressed.jpg";
		imwrite(name, bw, compression_params);	
		//process Image
		i++;
		//send Image
		Queue_send toSend(name);
		SharedVars::infosendLock.lock();
		SharedVars::infosend.push_back(toSend);
		SharedVars::infosendLock.unlock();
		SharedVars::infosendCv.notify_one();
		cout << "sleeping" << endl;
		sleep(4);
	}
}
