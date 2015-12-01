#include "RoadDetection.h"

using namespace std;
using namespace cv;

void imageQueueBuilder_main(){
	while(true){	
		Mat currentPic = takePic();
		imwrite("uncompressed.jpg", currentPic);
		Mat smallerPic;
		Size picSize;
		picSize.height = 192;
		picSize.width = 340;
		resize(currentPic, smallerPic, picSize);
		Mat bw;
		cvtColor(smallerPic, bw, CV_RGB2GRAY);
		vector<int> compression_params;
    		compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    		compression_params.push_back(50);
		imwrite("compressed.jpg", bw, compression_params);	
		//process Image

		//send Image
	}
}
