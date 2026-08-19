#include "telemetry.h"
#include <iostream>
#include <iomanip>

void Telemetry::setTelemetry(double alt, double spd, int bat,
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
    std::cout << "  Satellites : " << data.satellites << "\n";
    std::cout << "===================================\n";
}
