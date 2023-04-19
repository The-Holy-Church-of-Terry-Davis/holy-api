#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>

enum LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    Logger(const std::string& filename, LogLevel level);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);

private:
    void log(LogLevel level, const std::string& message);
    std::ofstream file_;
    LogLevel level_;
    const std::string levelName[4] = {"DEBUG", "INFO", "WARNING", "ERROR"};
};

#endif
