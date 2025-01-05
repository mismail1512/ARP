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
#include "DDSSubscriber.hpp"
#include "DDSPublisher.hpp"
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

void pubTarget(DDSPublisher<Targets,TargetsPubSubType>* mypub,Point targetsToSend[target_number]){
    Targets msg;
    std::vector<int32_t> targets_x{};
    std::vector<int32_t> targets_y{};
    for(int i=0;i<target_number;i++){
        targets_x.push_back(targetsToSend[i].x);
        targets_y.push_back(targetsToSend[i].y);
    }
    msg.targets_number(target_number);
    msg.targets_x(targets_x);
    msg.targets_y(targets_y);
    mypub->publish(msg);
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
    

    DDSPublisher<Targets,TargetsPubSubType>* mypub = new DDSPublisher<Targets,TargetsPubSubType>();
    mypub->init(TARGETS_TOPIC_NAME);
    mypub->waitSub();
    Point drone_position{DRONE_POS_X,DRONE_POS_Y};
    // Reading the first world state and initializing the objects generator
    WindowBorders borders{START_X,START_Y,WIDTH,HEIGHT};    
    ObjectsGenerator targets_obj_gen{borders.startX, borders.startX + borders.width - 1, borders.startY, borders.startY + borders.height - 1, target_number};

    Point targetsToSend[target_number];
    std::vector<Point> targets{};
    // Generating targets
    targets = targets_obj_gen.genrateObjects(std::vector<Point>{drone_position});
    std::cout << "targets generated" << std::endl;
    int i = 0;
    for (const Point& point : targets) {
        targetsToSend[i] = point;
        i++;
    }
    i = 0;

    while (true) {
        // Sending targets
        // write(targets_to_board_fd, &targetsToSend, sizeof(targetsToSend));
        pubTarget(mypub,targetsToSend);
        while (shouldPause.load()) {

            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait until resume signal is received
        }

        // Log heartbeat after sending targets
        log.logHeartbeat();

        usleep(UPDATE_TIME);
        
        // Read current world state
        // read(board_to_targets_fd, &worldState, sizeof(worldState));

        // drone_position.x = round(worldState.getDronePos().x);
        // drone_position.y = round(worldState.getDronePos().y);

        // // Check if the drone is on the current target
        // if (drone_position == targetsToSend[i]) {
        //     // Remove the target
        //     targetsToSend[i] = {NAN, NAN};
        //     i++;
        // }
    }

    delete mypub;
    return 0;
}
