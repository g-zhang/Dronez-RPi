#ifndef containers_h
#define containers_h

enum FlightMode {
    MANUAL,
    GPS,
    CVMODE,
    LAND,
    HOME
};
//public enum FlightMode { manualFlight, gpsNav, roadLineDetection, returnHome, landNow};
struct Vector3d {
    double x;
    double y;
    double z;
};

struct StatusPayload{
   FlightMode flightMode;
   int batteryLevel;
   Vector3d gyroData;
   Vector3d accData;
   Vector3d magData;
   Vector3d currentGPS;
};

#endif
