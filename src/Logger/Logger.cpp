//
// Created by mltac on 4/29/2025.
//

#include "Logger.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>

// Define the static member
std::ofstream Logger::logFile;

void Logger::Init() {
    logFile.open("2DGameEngine.log", std::ios::app); // Open log file in append mode
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file: 2DGameEngine.log" << std::endl;
    }
}

void Logger::Shutdown() {
    if (logFile.is_open()) {
        logFile.close(); // Ensure the log file is closed
    }
}

std::string Logger::CurrentDateTimeToString() {
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string output(30, '\0');
    std::strftime(&output[0], output.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(output.c_str()); // Remove null terminators
}

void Logger::Log(const std::string &message) {
    std::string output = "LOG: [" + CurrentDateTimeToString() + "] " + message;
    WriteToConsole(output, "\x1B[32m");  // Green text for log messages
    WriteToFile(output);
}

void Logger::Err(const std::string &message) {
    std::string output = "ERROR: [" + CurrentDateTimeToString() + "] " + message;
    WriteToConsole(output, "\x1B[91m");  // Red text for error messages
    WriteToFile(output);
}

void Logger::Warn(const std::string &message) {
    std::string output = "WARNING: [" + CurrentDateTimeToString() + "] " + message;
    WriteToConsole(output, "\x1B[33m");  // Yellow text for warnings
    WriteToFile(output);
}

void Logger::WriteToConsole(const std::string &message, const std::string &colorCode) {
    std::cout << colorCode << message << "\033[0m" << std::endl;
    std::cout.flush();
}

void Logger::WriteToFile(const std::string &message) {
    if (logFile.is_open()) {
        logFile << message << std::endl;
        logFile.flush();
    }
}