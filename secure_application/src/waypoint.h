#ifndef WAYPOINT_H
#define WAYPOINT_H

#include <string>
#include <vector>

// =====================================================
// VULNERABILITY: Buffer Overflow (C-style fixed buffer)
// - Waypoint name copied into a fixed 16-byte buffer
//   with no bounds checking (strcpy).
// =====================================================

struct Waypoint {
    char name[16];   // VULNERABLE: fixed-size C buffer
    double latitude;
    double longitude;
    double altitude;
};

class WaypointManager {
public:
    void uploadWaypoint(const std::string& name, double lat, double lon, double alt);
    void listWaypoints() const;
    const std::vector<Waypoint>& getWaypoints() const;
    void clearWaypoints();

private:
    std::vector<Waypoint> waypoints;
};

#endif
