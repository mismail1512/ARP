#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <string>
#include <ctime>
#include <filesystem>
#include <unistd.h>
#include <csignal>
#include <vector>
#include <algorithm>

#include <sys/types.h>

namespace fs = std::filesystem;

#include <atomic>
#include <chrono>
#include <thread>

// Atomic flag to indicate if the process should pause
std::atomic<bool> shouldPause(false);

<<<<<<< HEAD
void handlePauseSignal(int signal) {
    if (signal == SIGUSR1) { // 'p'
        //std::cout << "Window process: Received 'p'. Pausing..." << std::endl;
        shouldPause.store(true);
    }
}

void handleResumeSignal(int signal) {
    if (signal == SIGUSR2) { // 'st'
        //std::cout << "Window process: Received 'st'. Resuming..." << std::endl;
        sleep(5);
        shouldPause.store(false);
    }
}


=======
void handlePauseResumeSignal(int signal) {
    if (signal == SIGUSR1) { // 'p'
        // Toggle the pause state
        bool current = shouldPause.load();
        shouldPause.store(!current); // Flip the state

       
    }
}

>>>>>>> 58c11fb (Initial commit for my project)

// Helper function to read the last heartbeat from a log file
std::time_t get_last_heartbeat(const std::string& logfile, pid_t& pid) {
    std::ifstream log_file(logfile, std::ios::in);
    if (!log_file.is_open()) {
        std::cerr << "Failed to open log file: " << logfile << std::endl;
        return 0; // No valid timestamp
    }


    std::string line;
    std::time_t last_heartbeat = 0;

    while (std::getline(log_file, line)) {

        if (line.find("HEARTBEAT") != std::string::npos) {
            std::time_t timestamp;
            if (std::sscanf(line.c_str(), "HEARTBEAT | PID: %d | Timestamp: %ld", &pid, &timestamp) == 2) {

                last_heartbeat = timestamp;
            }
        }
    }

    log_file.close();
    return last_heartbeat;
}

// Monitor all log files in a directory
bool monitor_logs(const std::string& log_directory, int timeout_seconds) {
    std::time_t current_time = std::time(nullptr);


    for (const auto& entry : fs::directory_iterator(log_directory)) {
        
        if (entry.is_regular_file()) {
            const std::string logfile = entry.path().string();
            pid_t pid = 0;
            std::time_t last_heartbeat = get_last_heartbeat(logfile, pid);

            if (last_heartbeat == 0) {
                std::cerr << "No valid heartbeat in log: " << logfile << std::endl;
                continue;
            }

            if (current_time - last_heartbeat > timeout_seconds) {
                std::cerr << "Inactivity detected in log: " << logfile 
                          << " (Last heartbeat: " << last_heartbeat 
                          << ", Timeout: " << timeout_seconds << " seconds)." << std::endl;
                return true; // Trigger termination
            }
        }
    }
    return false; // No issues found
}

// Terminate all processes
void terminate_all_processes(const std::string& log_directory) {
    std::cerr << "Terminating all processes..." << std::endl;

    for (const auto& entry : fs::directory_iterator(log_directory)) {
        if (entry.is_regular_file()) {
            pid_t pid = 0;
            get_last_heartbeat(entry.path().string(), pid);
            if (pid > 0) {
                if (kill(pid, SIGTERM) == 0) {
                    std::cout << "Terminated process with PID: " << pid << std::endl;
                } else {
                    std::cerr << "Failed to terminate process with PID: " << pid << std::endl;
                }
            }
        }
    }
}

int main() {
    sleep(10);
    std::string log_directory = "./logs"; // Directory containing log files
    int timeout_seconds = 5; // Timeout in seconds

    pid_t pid = getpid(); // Get the current process ID

    std::ofstream pidFile("/tmp/watchdog.pid");
    pidFile << pid;
    pidFile.close();

<<<<<<< HEAD
    // Register the signal handlers
    signal(SIGUSR1, handlePauseSignal);
    signal(SIGUSR2, handleResumeSignal);


   /** for (const auto& entry : fs::directory_iterator(log_directory)) {
        if (fs::is_regular_file(entry)) {
            // Monitor each log file
           std::cout << entry.path()<< std::endl;
        }
    }*/
=======
    // Register the signal handler
    signal(SIGUSR1, handlePauseResumeSignal);
>>>>>>> 58c11fb (Initial commit for my project)


    // Ensure the log directory exists
    if (!fs::exists(log_directory)) {
        std::cerr << "Log directory does not exist. Creating..." << std::endl;
        fs::create_directory(log_directory);
    }

    std::cout << "Watchdog monitoring logs in: " << log_directory 
              << " with timeout of " << timeout_seconds << " seconds." << std::endl;

    while (true) {
        while (shouldPause.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait until resume signal is received
<<<<<<< HEAD
        }
=======
            
        }
        sleep(2);
>>>>>>> 58c11fb (Initial commit for my project)
        if (monitor_logs(log_directory, timeout_seconds)) {
            terminate_all_processes(log_directory);
            exit(1); // Exit after terminating all processes
            
              sleep(1);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Periodic check
    }

    return 0;
}
