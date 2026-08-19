#include "waypoint.h"
#include <iostream>
#include <iomanip>
#include <cstring>   // strcpy — intentionally unsafe

void WaypointManager::uploadWaypoint(const std::string& name,
                                     double lat, double lon, double alt) {
    Waypoint wp;

    // VULNERABILITY 2: Buffer Overflow (C-style)
    // wp.name is only 16 bytes. strcpy() performs NO
    // bounds check. A name longer than 15 chars will
    // overflow adjacent stack/heap memory, potentially
    // corrupting the waypoint struct or crashing.
    // Fix would be: strncpy(wp.name, name.c_str(), 15);
    //               wp.name[15] = '\0';
    strcpy(wp.name, name.c_str());  // VULNERABLE

    wp.latitude  = lat;
    wp.longitude = lon;
    wp.altitude  = alt;

    waypoints.push_back(wp);
    std::cout << "[WAYPOINT] Uploaded: " << wp.name
              << " (" << lat << ", " << lon << ") @ " << alt << "m\n";
}

void WaypointManager::listWaypoints() const {
    if (waypoints.empty()) {
        std::cout << "[WAYPOINT] No waypoints loaded.\n";
        return;
    }
    std::cout << "\n--- Waypoint List (" << waypoints.size() << " total) ---\n";
    std::cout << std::fixed << std::setprecision(4);
    for (size_t i = 0; i < waypoints.size(); ++i) {
        std::cout << "  [" << i + 1 << "] " << waypoints[i].name
                  << "  Lat=" << waypoints[i].latitude
                  << "  Lon=" << waypoints[i].longitude
                  << "  Alt=" << waypoints[i].altitude << "m\n";
    }
}

const std::vector<Waypoint>& WaypointManager::getWaypoints() const {
    return waypoints;
}

void WaypointManager::clearWaypoints() {
    waypoints.clear();
    std::cout << "[WAYPOINT] All waypoints cleared.\n";
}
