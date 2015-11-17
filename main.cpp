#include <thread>
#include <iostream>
#include "containers.h"
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

int main(){
    init();
    //start threads
    std::thread AutoFlightThread(autoflight_main);
    std::thread XbeeInterfaceThread(xbee_main);
    thread Xbee_read_in(read_in);
    //send_pic("woo.jpg");
    //loc_t gps_data;
    //gps_location(&gps_data);
    //cout<<gps_data.speed<<endl;
    //send_data(&gps_data, sizeof(gps_data), 'g');
    //wait for threads to exit
    XbeeInterfaceThread.join();
    AutoFlightThread.join();

    return 0;
}
