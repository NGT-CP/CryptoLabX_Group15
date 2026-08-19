#ifndef LOGGER_H
#define LOGGER_H

#include <string>

class Logger {
public:
    void log(const std::string& message);

    // VULNERABLE: filename comes from untrusted user input
    void setLogFile(const std::string& filename);
    void showLogs() const;

private:
    std::string logFile = "drone_log.txt";
};

#endif
