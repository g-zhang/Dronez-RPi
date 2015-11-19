#ifndef xbee_parsing_H
#define xbee_parsing_H

#include "../containers.h"

void gps_parse(const char *data,const int size);

void flight_mode(const char *data,const int size);

struct gps_info {
    double latitude;
    double longitude;
    double speed;
    double altitude;
    double course;
};

#endif
