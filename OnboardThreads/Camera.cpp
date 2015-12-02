#include "Camera.h"

using namespace std;
using namespace cv;

void initCamera(){
	/*
        SharedVars::Camera.set ( CV_CAP_PROP_FRAME_WIDTH,1920  );
        SharedVars::Camera.set ( CV_CAP_PROP_FRAME_HEIGHT, 1080 );
        SharedVars::Camera.set ( CV_CAP_PROP_BRIGHTNESS,75  );
        SharedVars::Camera.set ( CV_CAP_PROP_CONTRAST ,100  );
        SharedVars::Camera.set ( CV_CAP_PROP_SATURATION, 50  );
        SharedVars::Camera.set ( CV_CAP_PROP_GAIN, 50  );

        cout<<"Connecting to camera"<<endl;

        if ( !SharedVars::Camera.open() ) {
                cerr<<"Error opening camera"<<endl;
                return;
        }

        cout<<"Connected to camera ="<<SharedVars::Camera.getId() <<endl;
	*/


}

Mat takePic() {
	/*
        cv::Mat image;
        cout<<"Capturing"<<endl;
	SharedVars::CameraLock.lock();
	for(int i = 0; i < 31; ++i){
                SharedVars::Camera.retrieve ( image );
                if(i ==30){
			cout << "****************************Camera Snapping pic**********************" << endl;
                        cv::imwrite ("image"+std::to_string(1)+".jpg",image );
		}
        }
        SharedVars::CameraLock.unlock();
	*/
	SharedVars::cameraLock.lock();
	system("raspistill -o out.jpg -t 1");
	SharedVars::cameraLock.unlock();
	return imread("out.jpg");
}

char * matToBytes(Mat image)
{
   int size = image.total() * image.elemSize();
   char *bytes = new char[size];
   std::memcpy(bytes,image.data,size * sizeof(char));
   return bytes;
}
