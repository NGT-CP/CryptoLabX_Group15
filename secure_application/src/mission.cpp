#include "mission.h"
#include <iostream>
#include <cstdlib>   // system() — intentionally unsafe

void MissionController::startMission(const std::string& missionName,
                                     const WaypointManager& wpm) {
    if (wpm.getWaypoints().empty()) {
        std::cout << "[MISSION] Cannot start: no waypoints uploaded.\n";
        return;
    }

    activeMission  = missionName;
    missionActive  = true;

    std::cout << "[MISSION] Starting mission: " << missionName << "\n";

    // --------------------------------------------------
    // VULNERABILITY 3: Command Injection
    // missionName is taken directly from user input and
    // concatenated into a shell command. An attacker can
    // supply:  myMission; cat /etc/passwd
    // or:      myMission && rm -rf /tmp/logs
    // to execute arbitrary OS commands.
    // Fix: remove system() entirely, or sanitise/quote
    // the input and use exec-family calls with argv[].
    // --------------------------------------------------
    std::string cmd = "echo [MISSION-LOG] Mission started: " + missionName;
    std::cout << "[MISSION] Executing system log command...\n";
    system(cmd.c_str());   // VULNERABLE

    // Simulate visiting each waypoint
    const auto& waypoints = wpm.getWaypoints();
    for (size_t i = 0; i < waypoints.size(); ++i) {
        std::cout << "[MISSION] Navigating to waypoint [" << i + 1
                  << "]: " << waypoints[i].name << "\n";
    }

    std::cout << "[MISSION] Mission execution complete.\n";
    missionActive = false;
}

void MissionController::abortMission() {
    if (!missionActive) {
        std::cout << "[MISSION] No active mission to abort.\n";
        return;
    }
    missionActive = false;
    std::cout << "[MISSION] Mission '" << activeMission << "' aborted.\n";
    activeMission = "";
}

bool MissionController::isMissionActive() const {
    return missionActive;
}
