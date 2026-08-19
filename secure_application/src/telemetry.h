#ifndef TELEMETRY_H
#define TELEMETRY_H


struct TelemetryData {
    double altitude;     
    double speed;        
    int    battery;      
    double latitude;
    double longitude;
    int    satellites;
};

class Telemetry {
public:
    // VULNERABILITY: Improper Input Validation
    // Values are accepted without range validation.
    void setTelemetry(double alt, double spd, int bat,
                      double lat, double lon, int sats);
    void display() const;

private:
    TelemetryData data = {0, 0, 100, 0.0, 0.0, 0};
};

#endif
