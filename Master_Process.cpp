#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <cstdlib>
#include <ctime>

void start_process(const char* executable_path) {
    pid_t pid = fork(); 
    if (pid < 0) {
        // Fork failed
        std::cerr << "Failed to fork process" << std::endl;
        exit(1);
    } else if (pid == 0) {
        // Child process
        std::cout << "Starting process with PID: " << getpid() << std::endl;
        execl(executable_path, executable_path, nullptr); // Replace process image with the executable

        // If exec fails
        std::cerr << "Failed to start process: " << executable_path << std::endl;
        exit(1);
    }
    // Parent process continues
}

int main() {
    std::vector<const char*> executables = {
        "./watchdog",
        "./dynamics_server",
        //"./window",
        "./obstacles_gen",
        "./targets_gen",
        "./board"
    };

    std::cout << "Master process PID: " << getpid() << std::endl;

    // Start each process by forking
    for (const auto& executable : executables) {
        start_process(executable);
    }

    // Wait for all child processes with a timeout
    for (size_t i = 0; i < executables.size(); ++i) {
        int status;
        pid_t finished_pid = waitpid(-1, &status, WNOHANG); // Use WNOHANG for non-blocking wait
        if (finished_pid == -1) {
            std::cerr << "Error while waiting for child process." << std::endl;
            break;
        }

        if (finished_pid == 0) {
            std::cout << "Process is still running. Trying again." << std::endl;
            sleep(1);  // Sleep for a while before checking again
        } else {
            if (WIFEXITED(status)) {
                std::cout << "Process with PID " << finished_pid
                          << " exited with status " << WEXITSTATUS(status) << std::endl;
            } else {
                std::cerr << "Process with PID " << finished_pid << " terminated abnormally." << std::endl;
            }
        }
    }

    std::cout << "All child processes finished. Master process exiting." << std::endl;
    return 0;
}
