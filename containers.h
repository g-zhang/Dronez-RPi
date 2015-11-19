#ifndef containers_h
#define containers_h

enum FlightMode {
    MANUAL,
    GPS,
    CVMODE,
    LAND,
    home
};
//public enum FlightMode { manualFlight, gpsNav, roadLineDetection, returnHome, landNow};
struct Vector3d {
    double x;
    double y;
    double z;
};


#endif
