#include <unistd.h>
#include "world_state.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <algorithm>
#include "objects_generator.hpp"
#include "configs.hpp"
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
    }
}

int main(int argc, char *argv[] ) {
   pid_t pid = getpid(); // Get the current process ID
    logger log("./logs/targets_gen.log", pid); // Initialize logger for this process with a unique log file

std::ofstream pidFile("/tmp/targ.pid");
    pidFile << pid;
    pidFile.close();

    // Register the signal handlers
    signal(SIGUSR1, handlePauseSignal);
    signal(SIGUSR2, handleResumeSignal);

    // Initialization
    // Making pipes
    mkfifo(targets_to_board_pipe, 0666);
    int targets_to_board_fd = open(targets_to_board_pipe, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    mkfifo(board_to_targets_pipe, 0666);
    int board_to_targets_fd = open(board_to_targets_pipe, O_RDONLY | O_CREAT | O_TRUNC, 0666);

    Point drone_position;
    // Reading the first world state and initializing the objects generator
    WorldState<obstacles_number, target_number> worldState{drone_position};
    read(board_to_targets_fd, &worldState, sizeof(worldState));

    WindowBorders borders{worldState.getBorder()};
    ObjectsGenerator targets_obj_gen{borders.startX, borders.startX + borders.width - 1, borders.startY, borders.startY + borders.height - 1, target_number};

    Point targetsToSend[target_number];
    std::vector<Point> targets{};
    // Generating targets
    targets = targets_obj_gen.genrateObjects(std::vector<Point>{worldState.getDronePos()});

    int i = 0;
    for (const Point& point : targets) {
        targetsToSend[i] = point;
        i++;
    }
    i = 0;

    while (true) {
        // Sending targets
        write(targets_to_board_fd, &targetsToSend, sizeof(targetsToSend));

        while (shouldPause.load()) {

            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait until resume signal is received
        }

        // Log heartbeat after sending targets
        log.logHeartbeat();

        usleep(UPDATE_TIME);
        
        // Read current world state
        read(board_to_targets_fd, &worldState, sizeof(worldState));

        drone_position.x = round(worldState.getDronePos().x);
        drone_position.y = round(worldState.getDronePos().y);

        // Check if the drone is on the current target
        if (drone_position == targetsToSend[i]) {
            // Remove the target
            targetsToSend[i] = {NAN, NAN};
            i++;
        }
    }

    close(targets_to_board_fd);
    close(board_to_targets_fd);
    return 0;
}
