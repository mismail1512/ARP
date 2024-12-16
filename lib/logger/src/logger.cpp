#include "logger.hpp"
#include <fcntl.h>    // For O_CREAT, O_WRONLY, O_APPEND
#include <sys/stat.h> // For S_IRUSR, S_IWUSR
#include <unistd.h>   // For open(), write(), close()
#include <cstring>    // For strlen()
#include <iostream>
#include <chrono>
#include <ctime>      // For ctime()

logger::logger(const char* logfile, pid_t pid) : log_file_path(logfile), process_pid(pid) {
    
    int fd = open(logfile, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR,0666);
    if (fd < 0) {
        std::cerr << "Failed to open log file: " << logfile << std::endl;
        exit(1);
    }
    close(fd); // Close the file immediately after opening for creation check
}

void logger::logHeartbeat() {
    // Get the current timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);

    // Format the log message
    std::string log_message = "HEARTBEAT | PID: " + std::to_string(process_pid) +
                              " | Timestamp: " +std::to_string(time_now) + "\n";


    // Open the log file, write the message, and close it
    int fd = open(log_file_path.c_str(), O_WRONLY | O_APPEND,0666);
    if (fd < 0) {
        std::cerr << "Failed to open log file: " << log_file_path << std::endl;
        return;
    }
    // Write the log message to the file
    int res = write(fd, log_message.c_str(), log_message.size());
    if(res==-1){
        std::cerr << "Failed to write in log file: " << log_file_path << std::endl;
        return;
    }

    close(fd);
}
