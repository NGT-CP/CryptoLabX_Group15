#include "telemetry.h"
#include <iostream>
#include <iomanip>

void Telemetry::setTelemetry(double alt, double spd, int bat,
<<<<<<< HEAD
                              double lat, double lon, int sats) {
    // VULNERABILITY 4: Improper Input Validation
    // No bounds checking is performed on any value.
    // An attacker or faulty sensor could supply:
    //   altitude = -9999  (negative altitude accepted)
    //   battery  = 999    (impossible percentage)
    //   speed    = 99999  (far beyond drone capability)
    // These would be silently accepted, potentially
    // causing incorrect autopilot decisions or crashes.
    // Fix: validate each field before assignment.
    data.altitude   = alt;   // should check: 0 <= alt <= 500
    data.speed      = spd;   // should check: 0 <= spd <= 100
    data.battery    = bat;   // should check: 0 <= bat <= 100
    data.latitude   = lat;
    data.longitude  = lon;
    data.satellites = sats;
}

void Telemetry::display() const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n========= DRONE TELEMETRY =========\n";
    std::cout << "  Altitude   : " << data.altitude   << " m\n";
    std::cout << "  Speed      : " << data.speed      << " km/h\n";
    std::cout << "  Battery    : " << data.battery    << " %\n";
    std::cout << "  Latitude   : " << data.latitude   << "\n";
    std::cout << "  Longitude  : " << data.longitude  << "\n";
=======
                             double lat, double lon, int sats)
{

    data.altitude = alt; // should check: 0 <= alt <= 500
    data.speed = spd;    // should check: 0 <= spd <= 100
    data.battery = bat;  // should check: 0 <= bat <= 100
    data.latitude = lat;
    data.longitude = lon;
    data.satellites = sats;
}

void Telemetry::display() const
{
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n========= DRONE TELEMETRY =========\n";
    std::cout << "  Altitude   : " << data.altitude << " m\n";
    std::cout << "  Speed      : " << data.speed << " km/h\n";
    std::cout << "  Battery    : " << data.battery << " %\n";
    std::cout << "  Latitude   : " << data.latitude << "\n";
    std::cout << "  Longitude  : " << data.longitude << "\n";
>>>>>>> origin/main
    std::cout << "  Satellites : " << data.satellites << "\n";
    std::cout << "===================================\n";
}
