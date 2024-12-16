#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include "commands.hpp"
#include <string.h>
#include <unordered_map>
#include "configs.hpp"
#include "logger.hpp" // Include the logger header
#include <signal.h>   // For kill() and signal constants
#include <sys/types.h> // For pid_t

#include <fstream>
#include <csignal>


Command AssignCommand(const std::string& str) {
    const std::unordered_map<std::string, Command> cmdMap = {
        {"w", Command::w},
        {"e", Command::e},
        {"r", Command::r},
        {"s", Command::s},
        {"d", Command::d},
        {"f", Command::f},
        {"x", Command::x},
        {"c", Command::c},
        {"v", Command::v},
        {"p", Command::p},
        {"st", Command::st},
        {"reset", Command::reset},
    };
    auto it = cmdMap.find(str);

    if (it != cmdMap.end()) 
        return it->second;
    return Command::UNKNOWN;
}

int main() {
   // pid_t pid = getpid();  // Get process ID
  //  logger log("./logs/input.log", pid); // Initialize logger for this process with a unique log file
  
    mkfifo(input_to_board_pipe, 0666);
    int input_to_board_pipe_fd = open(input_to_board_pipe, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    Command cmd;
    
    std::string inputString;  

    while (true) {
        std::cout << "Enter one of the following commands to move\n w,e,r,\ns,d,f,\nx,c,v\n" << std::endl;
        std::cout << "p to pause\nst to start again\nreset to reset" << std::endl;
        std::cin >> inputString;
        
        // Log heartbeat to indicate that the process is still active
       // log.logHeartbeat();  // Log heartbeat at each iteration to ensure the watchdog monitors this process
        
        cmd = AssignCommand(inputString);
        
        
        if (cmd == Command::UNKNOWN)
            continue;
    pid_t windowProcessPID1;
    pid_t windowProcessPID2;
    pid_t windowProcessPID3;
    pid_t windowProcessPID4;
    pid_t windowProcessPID5;
    pid_t windowProcessPID6;
    std::ifstream pidFile1("/tmp/window_process.pid");
    std::ifstream pidFile2("/tmp/dynamic.pid");
    std::ifstream pidFile3("/tmp/obst.pid");
    std::ifstream pidFile4("/tmp/targ.pid");    
    std::ifstream pidFile5("/tmp/board.pid");    
    std::ifstream pidFile6("/tmp/watchdog.pid");    

    if (cmd == Command::p || cmd == Command::st|| cmd == Command::reset) {
    

    if (pidFile2 >> windowProcessPID2 && pidFile3 >> windowProcessPID3 && pidFile4 >> windowProcessPID4&& pidFile5 >> windowProcessPID5&& pidFile6 >> windowProcessPID6 ) {
        std::cout << "Read Window Process PID: " << windowProcessPID2<< ", " << windowProcessPID3<< ", " <<  windowProcessPID4<<", " <<  windowProcessPID5<<std::endl;

        // Send the appropriate signal based on the command
        if (cmd == Command::p) {
            // Pause the window process
            if ((kill(windowProcessPID2, SIGUSR1) == 0)&& (kill(windowProcessPID3, SIGUSR1) == 0)&& (kill(windowProcessPID6, SIGUSR1) == 0)&& (kill(windowProcessPID6, SIGUSR1) == 0)) {
                std::cout << "Signal sent successfully to pause process (PID: " <<windowProcessPID2<< ", " << windowProcessPID3<< ", " <<  windowProcessPID4<<", " <<  windowProcessPID5<<std::endl;
            } else {
                perror("Failed to send pause signal");
            }
        } else if (cmd == Command::st) {
            // Resume the window process
            if ((kill(windowProcessPID2, SIGUSR2) == 0)&& (kill(windowProcessPID3, SIGUSR2) == 0)&& (kill(windowProcessPID4, SIGUSR2) == 0&& (kill(windowProcessPID6, SIGUSR2) == 0))) {
                std::cout << "Signal sent successfully to start process (PID: " << windowProcessPID2<< ", " << windowProcessPID3<< ", " <<  windowProcessPID4<<", " <<  windowProcessPID5<<std::endl;
            } 
            
            else {
                perror("Failed to send resume signal");
            }
        }

    else if (cmd == Command::reset)
{
std::cout << "khjvgjcch gb " << std::endl;

// reset the window process
if (kill(windowProcessPID5, SIGUSR1) == 0)
 {
std::cout << "Signal sent successfully to reset process (PID: " << windowProcessPID5<<std::endl;
} 
else {
perror("Failed to send reset signal");
}
}
    } 
    else {
        std::cerr << "Failed to read window process PID. Ensure the window process is running." << std::endl;
    }



}

        // Send the command to the board
        write(input_to_board_pipe_fd, &cmd, sizeof(cmd));

    }

    close(input_to_board_pipe_fd);
    return 0;
}
