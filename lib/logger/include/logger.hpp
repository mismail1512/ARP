#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <chrono>
#include <ctime>
#include <unistd.h>
#include <fcntl.h> // For open(), O_CREAT, etc.

class logger {
public:
    logger(const char* logfile, pid_t pid);
    void logHeartbeat(); // Logs the "alive" state of the process

private:
    std::string log_file_path; // Store the log file path as a string
    pid_t process_pid;         // Process ID
};

#endif // LOGGER_HPP
