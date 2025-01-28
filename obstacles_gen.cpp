#include <unistd.h>
#include "world_state.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <algorithm>
#include "objects_generator.hpp"
#include "configs.hpp"
#include <random>
#include "logger.hpp" // Include the logger header
#include <signal.h>   // For kill() and signal constants
#include <sys/types.h> // For pid_t

#include <fstream>
#include <csignal>
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
        shouldPause.store(false);
=======
void handlePauseResumeSignal(int signal) {
    if (signal == SIGUSR1) { // 'p'
        // Toggle the pause state
        bool current = shouldPause.load();
        shouldPause.store(!current); // Flip the state
>>>>>>> 58c11fb (Initial commit for my project)
    }
}


int main(int argc, char *argv[] ) {
    pid_t pid = getpid(); // Get the current process ID
    logger log("./logs/obstacles_gen.log", pid); // Initialize logger for this process with a unique log file

std::ofstream pidFile("/tmp/obst.pid");
    pidFile << pid;
    pidFile.close();

<<<<<<< HEAD
    // Register the signal handlers
    signal(SIGUSR1, handlePauseSignal);
    signal(SIGUSR2, handleResumeSignal);
=======
    // Register the signal handler
    signal(SIGUSR1, handlePauseResumeSignal);

>>>>>>> 58c11fb (Initial commit for my project)

    std::random_device rd;  // Non-deterministic random seed
    std::mt19937 gen(rd()); // Mersenne Twister RNG
    Point drone_position{0,0};
    // Define a uniform real distribution between 0 and 1
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    // Initialization
    Point obstaclesToSend[obstacles_number];
    // Make the pipes
    mkfifo(obstacles_to_board_pipe, 0666);
    int obstacles_to_board_pipe_fd = open(obstacles_to_board_pipe, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    mkfifo(board_to_obstacles_pipe, 0666);
    int board_to_obstacles_fd = open(board_to_obstacles_pipe, O_RDONLY | O_CREAT | O_TRUNC, 0666);

    // Read first world state
    WorldState <obstacles_number, target_number> worldState{drone_position};
    read(board_to_obstacles_fd, &worldState, sizeof(worldState));

    // Initialize the obstacles generator
    WindowBorders borders = worldState.getBorder();
    ObjectsGenerator obstacles_obj_gen{borders.startX, borders.startX + borders.width - 1, borders.startY, borders.startY + borders.height - 1, obstacles_number};
    
    // Collect except points (drone's position and targets)
    std::vector<Point> except_points{worldState.getDronePos()};
    for (const Point& target : worldState.targets_positions) {
        except_points.push_back(target);
    }
        
    // Generate obstacles
    std::vector<Point> obstacles = obstacles_obj_gen.genrateObjects(except_points);
    
    int i = 0;
    for (const Point& point : obstacles) {
        obstaclesToSend[i] = point;
        i++;
    }

    i = 0;
    while (true) {
        // Log heartbeat to indicate activity
        log.logHeartbeat(); // Log heartbeat
        while (shouldPause.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait until resume signal is received
        }
        // Sending obstacles to the board
        write(obstacles_to_board_pipe_fd, &obstaclesToSend, sizeof(obstaclesToSend));

        // Sleep for a while before next update
        usleep(500 * UPDATE_TIME);

        // Read current world state from the board
        read(board_to_obstacles_fd, &worldState, sizeof(worldState));

        // Adjust drone position (round to nearest integer)
        drone_position.x = round(worldState.getDronePos().x);
        drone_position.y = round(worldState.getDronePos().y);

        // Rebuild the except points list and generate new obstacles
        except_points.clear();
        except_points.push_back(drone_position);
        for (const Point& target : worldState.targets_positions) {
            except_points.push_back(target);
        }

        obstacles = obstacles_obj_gen.genrateObjects(except_points);

        i = 0;
        // Update obstacles to send based on new random values
        for (const Point& point : obstacles) {
            if (dis(gen) > 0.7) {
                obstaclesToSend[i] = point;
            }
            i++;
        }
    }

    // Close pipes
    close(obstacles_to_board_pipe_fd);
    close(board_to_obstacles_fd);
    return 0;
}
