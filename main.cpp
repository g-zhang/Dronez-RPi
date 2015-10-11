#include <thread>
#include <iostream>
#include "containers.h"
#include "OnboardThreads/XbeeInterface.h"
#include "OnboardThreads/AutoFlight.h"

using namespace std;

void init() {
    //init all shared vars
    cout << "Initializing variables" << endl;
    SharedVars::init();

    cout << "Initializing threads" << endl;
}

int main(){
    init();

    //start threads
    std::thread AutoFlightThread(autoflight_main);
    std::thread XbeeInterfaceThread(xbee_main);

    //wait for threads to exit
    XbeeInterfaceThread.join();
    AutoFlightThread.join();

    return 0;
}
