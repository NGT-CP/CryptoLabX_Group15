#ifndef TELEMETRY_H
#define TELEMETRY_H

// =====================================================
// Telemetry — displays simulated drone sensor data.
// Also demonstrates Improper Input Validation:
// - Altitude/battery inputs are not range-checked,
//   allowing nonsensical or dangerous values.
// =====================================================

struct TelemetryData {
    double altitude;     // meters  (valid: 0–500)
    double speed;        // km/h    (valid: 0–100)
    int    battery;      // percent (valid: 0–100)
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
