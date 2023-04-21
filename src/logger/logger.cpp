#include "logger.h"
#include <iomanip>
#include <sstream>
#include <chrono>

Logger::Logger(const std::string& filename, LogLevel level) {
    file_.open(filename);
    level_ = level;
}

void Logger::debug(const std::string& message) {
    log(DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(INFO, message);
}

void Logger::warning(const std::string& message) {
    log(WARNING, message);
}

void Logger::error(const std::string& message) {
    log(ERROR, message);
}

void Logger::log(LogLevel level, const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    std::stringstream timeString;
    timeString << std::put_time(std::gmtime(&nowTime), "%F %T UTC");

    if (level >= level_) {
        std::cout << "[" << levelName[level] << "] " << "[" << timeString.str() << "] " << message << std::endl;
        file_ << "[" << levelName[level] << "] " << "[" << timeString.str() << "] " << message << std::endl;
    }
}
