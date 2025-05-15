//
// Created by mltac on 4/29/2025.
//

// Look into spdlog - github.com/gabime/spdlog

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream> // Include for file output

class Logger {
private:
    static std::ofstream logFile; // Static file stream for logging
    static std::string CurrentDateTimeToString(); // Helper to get current date/time
    static void WriteToConsole(const std::string &message, const std::string &colorCode);
    static void WriteToFile(const std::string &message);
public:
    static void Log(const std::string &message);
    static void Err(const std::string &message);
    static void Warn(const std::string &message);
    static void Init(); // Initialize (open log file)
    static void Shutdown(); // Close log file
};

#endif // LOGGER_H
