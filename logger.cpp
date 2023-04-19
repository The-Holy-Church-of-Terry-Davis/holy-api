#include "logger.h"

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
    if (level >= level_) {
	std::cout << "[" << levelName[level] << "] " << message << std::endl;
        file_ << "[" << levelName[level] << "] " << message << std::endl;
    }
}
