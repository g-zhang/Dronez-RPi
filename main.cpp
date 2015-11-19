#include <thread>
#include <iostream>
#include <fstream>
#include "containers.h"
#include "sharedvars.h"
#include "OnboardThreads/XbeeInterface.h"
#include "OnboardThreads/AutoFlight.h"
#include "OnboardThreads/gps.h"

using namespace std;

void init() {
    //init all shared vars
    cout << "Initializing variables" << endl;
    SharedVars::init();
    cout<<"Turning on GPS"<<endl;
    gps_init();
    cout << "Initializing threads" << endl;

}

void readArguments(int argc, char* argv[]) {
  if (argc > 1) {
    ifstream gpsplanfile;
    cout << argv[1] << endl;
    gpsplanfile.open(argv[1]);
    Vector3d current = { 0, 0, 0 };

    loc_t* data = GPS_info();
    current.x = data->latitude;
    current.y = data->longitude;
    cout << "Current lat: " << current.x << endl;
    cout << "Current long: " << current.y << endl;

    char type = 0;
    int num = 0;
    gpsplanfile >> type >> num;
    //cout << type << " " << num << endl;

    SharedVars::gpsFlightPlanLock.lock();

    for(int i = 0; i < num; i++) {
      double heading = 0.0;
      double distance = 0.0;
      gpsplanfile >> heading >> distance;
      cout << heading << " "<< distance << endl;
      Vector3d newpoint = findGPSPoint(current, heading, distance);
      SharedVars::gpsFlightPlan.push(newpoint);
      current = newpoint;
    }

      SharedVars::gpsFlightPlanLock.unlock();

    // while(!SharedVars::gpsFlightPlan.empty()) {
    //   cout << SharedVars::gpsFlightPlan.front().x << " " << SharedVars::gpsFlightPlan.front().y << endl;
    //   SharedVars::gpsFlightPlan.pop();
    // }

    gpsplanfile.close();
  }
}

int main(int argc, char* argv[]){
    init();
    readArguments(argc, argv);
    //start threads
    std::thread AutoFlightThread(autoflight_main);
    //std::thread XbeeInterfaceThread(xbee_main);
    //thread Xbee_read_in(read_in);

    //wait for threads to exit
    //XbeeInterfaceThread.join();
    AutoFlightThread.join();

    return 0;
}
