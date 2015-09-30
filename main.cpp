#include "sharedvars.h"
#include <thread>
#include <iostream>
#include "OnboardThreads/XbeeInterface.h"

using namespace std;

void init() {
    //init all shared vars
    cout << "initializing vars" << endl;
}

int main(){
    init();

    //start threads
    thread XbeeInterfaceThread(xbee_main);
    //XbeeInterfaceThread.detach();
    XbeeInterfaceThread.join();

    return 0;
}
