#include "logger.h"
#include <iostream>
#include <fstream>
#include <ctime>

void Logger::setLogFile(const std::string& filename) {
    logFile = filename;  // VULNERABLE — no sanitisation
    std::cout << "[LOGGER] Log file set to: " << logFile << "\n";
}

void Logger::log(const std::string& message) {

    std::time_t now = std::time(nullptr);
    char timebuf[32];
    std::strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    // VULNERABLE: opens whatever path logFile contains
    std::ofstream ofs(logFile, std::ios::app);
    if (!ofs.is_open()) {
        std::cerr << "[LOGGER] ERROR: Cannot open log file: " << logFile << "\n";
        return;
    }
    ofs << "[" << timebuf << "] " << message << "\n";
    ofs.close();
}

void Logger::showLogs() const {
    std::ifstream ifs(logFile);
    if (!ifs.is_open()) {
        std::cout << "[LOGGER] No log file found: " << logFile << "\n";
        return;
    }
    std::cout << "\n--- Log File: " << logFile << " ---\n";
    std::string line;
    while (std::getline(ifs, line)) {
        std::cout << line << "\n";
    }
    std::cout << "--- End of Log ---\n";
}
