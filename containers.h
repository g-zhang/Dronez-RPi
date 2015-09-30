#ifndef containers_h
#define containers_h

enum FlightMode {
    MANUAL,
    GPS,
    CVMODE,
    LAND
};

struct Coord3d {
    double x;
    double y;
    double z;
};


#endif
