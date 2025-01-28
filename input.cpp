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
<<<<<<< HEAD
        {"p", Command::p},
        {"st", Command::st},
=======
        {"ps", Command::ps},
>>>>>>> 58c11fb (Initial commit for my project)
        {"reset", Command::reset},
    };
    auto it = cmdMap.find(str);

    if (it != cmdMap.end()) 
        return it->second;
    return Command::UNKNOWN;
}

int main() {
   
    mkfifo(input_to_board_pipe, 0666);
    int input_to_board_pipe_fd = open(input_to_board_pipe, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    Command cmd;
    
    std::string inputString;  

    while (true) {
        std::cout << "Enter one of the following commands to move\n w,e,r,\ns,d,f,\nx,c,v\n" << std::endl;
<<<<<<< HEAD
        std::cout << "p to pause\nst to start again\nreset to reset" << std::endl;
=======
        std::cout << "ps to pause and to start again\nreset to reset" << std::endl;
>>>>>>> 58c11fb (Initial commit for my project)
        std::cin >> inputString;
        
        
        cmd = AssignCommand(inputString);
        if (cmd == Command::UNKNOWN)
            continue;
    
    pid_t dynmaicProcessPID;
    pid_t obsProcessPID;
    pid_t targProcessPID;
    pid_t boardProcessPID;
    pid_t watchdogProcessPID;
<<<<<<< HEAD
    //std::ifstream pidFile1("/tmp/window_process.pid");
=======
>>>>>>> 58c11fb (Initial commit for my project)
    std::ifstream pid_dynamic("/tmp/dynamic.pid");
    std::ifstream pid_obs("/tmp/obst.pid");
    std::ifstream pid_targ("/tmp/targ.pid");    
    std::ifstream pid_board("/tmp/board.pid");    
    std::ifstream pid_watchdog("/tmp/watchdog.pid");    

<<<<<<< HEAD
    if (cmd == Command::p || cmd == Command::st|| cmd == Command::reset) 
=======
    if (cmd == Command::ps ||  cmd == Command::reset) 
>>>>>>> 58c11fb (Initial commit for my project)
    {
    
            if (pid_dynamic >> dynmaicProcessPID && pid_obs >> obsProcessPID && pid_targ >> targProcessPID&& pid_board >> boardProcessPID&& pid_watchdog >> watchdogProcessPID ) 
            {
                std::cout << "Read Processes PIDs: " << dynmaicProcessPID<< ", " << obsProcessPID<< ", " <<  targProcessPID<<", " <<  boardProcessPID<<", " <<  watchdogProcessPID<<std::endl;

                // Send the appropriate signal based on the command
<<<<<<< HEAD
                if (cmd == Command::p) {
=======
                if (cmd == Command::ps) {
>>>>>>> 58c11fb (Initial commit for my project)
                    // Pause the window process
                    if ((kill(dynmaicProcessPID, SIGUSR1) == 0)&& (kill(obsProcessPID, SIGUSR1) == 0)&& (kill(targProcessPID, SIGUSR1) == 0&& (kill(watchdogProcessPID, SIGUSR1) == 0)&& (kill(boardProcessPID, SIGUSR1) == 0))) {
                        std::cout << "Signal sent successfully to pause process (PID: " << dynmaicProcessPID<< ", " << obsProcessPID<< ", " <<  targProcessPID<<", "<<  watchdogProcessPID<<std::endl;
                    } else {
                        perror("Failed to send pause signal");
                    }
<<<<<<< HEAD
                } else if (cmd == Command::st) {
                    // Resume the window process
                    if ((kill(dynmaicProcessPID, SIGUSR1) == 0)&& (kill(obsProcessPID, SIGUSR2) == 0)&& (kill(targProcessPID, SIGUSR2) == 0&& (kill(watchdogProcessPID, SIGUSR2) == 0)&& (kill(boardProcessPID, SIGUSR1) == 0))) {
                        std::cout << "Signal sent successfully to start process (PID: " << dynmaicProcessPID<< ", " << obsProcessPID<< ", " <<  targProcessPID <<", " <<  watchdogProcessPID<<std::endl;
                    } 
                    
                    else {
                        perror("Failed to send resume signal");
                    }
                }

=======
                } 
>>>>>>> 58c11fb (Initial commit for my project)
            else if (cmd == Command::reset)
            {

                    // reset the window process
                    if (kill(boardProcessPID, SIGUSR2) == 0&& (kill(dynmaicProcessPID, SIGUSR2) == 0))
                    {
                    std::cout << "Signal sent successfully to reset process (PID: " << boardProcessPID<<std::endl;
                    } 
                    else {
                    perror("Failed to send reset signal");
                    }
        }
            } 
            
            else {
                std::cerr << "Failed to read  process PID. Ensure the  processes are running." << std::endl;
            }

 
       }

        // Send the command to the board
        write(input_to_board_pipe_fd, &cmd, sizeof(cmd));

    }

    close(input_to_board_pipe_fd);
    return 0;
}
