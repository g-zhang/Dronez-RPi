#include "xbee_parsing.h"
#include <string.h>
#include <iostream>


using namespace std;

void gps_parse(const char *data,const int size){
	gps_info test;
	memcpy(&test, data, size);
}