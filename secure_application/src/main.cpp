#include <iostream>
#include <string>
#include <limits>

#include "auth.h"
#include "waypoint.h"
#include "mission.h"
#include "telemetry.h"
#include "logger.h"

using namespace std;

static void printBanner() {
    std::cout <<"=========> Welcome to dron application <=========="<<endl;
}

static void printMenu() {
    std::cout << "\n======= MAIN MENU =======\n";
    std::cout << "  1. Login\n";
    std::cout << "  2. Upload Waypoint\n";
    std::cout << "  3. List Waypoints\n";
    std::cout << "  4. Start Mission\n";
    std::cout << "  5. Abort Mission\n";
    std::cout << "  6. View Telemetry\n";
    std::cout << "  7. Set Telemetry (manual)\n";
    std::cout << "  8. Set Log File\n";
    std::cout << "  9. View Logs\n";
    std::cout << "  0. Logout / Exit\n";
    std::cout << "=========================\n";
    std::cout << "Choice: ";
}

static void clearInputBuffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}


int main() {
    printBanner();

    WaypointManager wpm;
    MissionController mc;
    Telemetry        telem;
    Logger           logger;


    telem.setTelemetry(120.0, 45.0, 87, 28.6139, 77.2090, 12);

    int choice = -1;

    while (true) {
        printMenu();
        std::cin >> choice;
        clearInputBuffer();

        if (choice == 0) {
            if (isLoggedIn) logout();
            std::cout << "[SYSTEM] Shutting down. Goodbye.\n";
            break;
        }

        // 1. Login
        if (choice == 1) {
            if (isLoggedIn) {
                std::cout << "[AUTH] Already logged in as: " << currentUser << "\n";
                continue;
            }
            std::string user, pass;
            std::cout << "Username: "; std::getline(std::cin, user);
            std::cout << "Password: "; std::getline(std::cin, pass);
            bool ok = login(user, pass);
            if (ok) logger.log("Login: " + user);
            continue;
        }

        //Auth guard 
        if (!checkAuth()) {
            std::cout << "[AUTH] Access denied. Please login first (option 1).\n";
            continue;
        }

        switch (choice) {

        // 2. Upload Waypoint
        case 2: {
            std::string name;
            double lat, lon, alt;
            std::cout << "Waypoint name (keep short!): ";
            std::getline(std::cin, name);
            std::cout << "Latitude  : "; std::cin >> lat;
            std::cout << "Longitude : "; std::cin >> lon;
            std::cout << "Altitude  : "; std::cin >> alt;
            clearInputBuffer();

            // VULNERABILITY: name is NOT length-checked before
            // being passed to strcpy() inside uploadWaypoint().
            wpm.uploadWaypoint(name, lat, lon, alt);
            logger.log("Waypoint uploaded: " + name);
            break;
        }

        //3. List Waypoints
        case 3:
            wpm.listWaypoints();
            break;

        //4. Start Mission
        case 4: {
            std::string mname;
            std::cout << "Mission name: ";
            std::getline(std::cin, mname);

            // VULNERABILITY: mname fed into system() inside
            // startMission() without any sanitisation.
            mc.startMission(mname, wpm);
            logger.log("Mission started: " + mname);
            break;
        }

        //5. Abort Mission
        case 5:
            mc.abortMission();
            logger.log("Mission aborted by " + currentUser);
            break;

        //6. View Telemetry
        case 6:
            telem.display();
            break;

        // 7. Set Telemetry (manual / demo)
        case 7: {
            double alt, spd, lat, lon;
            int bat, sats;
            std::cout << "Altitude (m)   : "; std::cin >> alt;
            std::cout << "Speed (km/h)   : "; std::cin >> spd;
            std::cout << "Battery (%)    : "; std::cin >> bat;
            std::cout << "Latitude       : "; std::cin >> lat;
            std::cout << "Longitude      : "; std::cin >> lon;
            std::cout << "Satellites     : "; std::cin >> sats;
            clearInputBuffer();

            // VULNERABILITY: values accepted without range validation
            telem.setTelemetry(alt, spd, bat, lat, lon, sats);
            logger.log("Telemetry updated manually");
            break;
        }

        // 8. Set Log File
        case 8: {
            std::string fname;
            std::cout << "Log file path: ";
            std::getline(std::cin, fname);

            // VULNERABILITY: fname is not sanitised —
            // path traversal (../../etc/passwd) is possible.
            logger.setLogFile(fname);
            break;
        }

        // 9. View Logs
        case 9:
            logger.showLogs();
            break;

        default:
            std::cout << "[SYSTEM] Unknown option.\n";
        }
    }

    return 0;
}
