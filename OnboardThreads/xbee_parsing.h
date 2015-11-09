#ifndef xbee_parsing_H
#define xbee_parsing_H

void gps_parse(const char *data,const int size);

struct gps_info {
    double latitude;
    double longitude;
    double speed;
    double altitude;
    double course;
};

#endif