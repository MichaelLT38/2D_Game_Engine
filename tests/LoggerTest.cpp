//
// Created by mltac on 5/11/2025.
//

#include "../src/Logger/Logger.h" // Make sure this is the correct path to your logger header
#include <iostream>
#include <fstream>
#include <cassert>

int main() {
    // Initialize the logger
    Logger::Init();

    // Log various messages
    Logger::Log("Test log message");
    Logger::Warn("Test warning message");
    Logger::Err("Test error message");

    // Confirm that the log file was created and can be opened
    {
        std::ifstream logFile("2DGameEngine.log");
        assert(logFile.is_open() && "Log file should be created and accessible.");
    }

    // Shutdown the logger
    Logger::Shutdown();

    std::cout << "LoggerTest completed successfully." << std::endl;
    return 0;
}