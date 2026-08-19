#ifndef MISSION_H
#define MISSION_H

#include "waypoint.h"
#include <string>

// VULNERABILITY: Command Injection
// - Mission name passed directly to system() call
//   without sanitisation, allowing shell injection.

class MissionController {
public:
    void startMission(const std::string& missionName,
                      const WaypointManager& wpm);
    void abortMission();
    bool isMissionActive() const;

private:
    bool missionActive = false;
    std::string activeMission;
};

#endif
