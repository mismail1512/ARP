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

void pubObstacle(DDSPublisher<Obstacles,ObstaclesPubSubType>* mypub,Point obstaclesToSend[obstacles_number]){
    Obstacles msg;
    std::vector<int32_t> obstacles_x{};
    std::vector<int32_t> obstacles_y{};
    for(int i=0;i<obstacles_number;i++){
        obstacles_x.push_back(obstaclesToSend[i].x);
        obstacles_y.push_back(obstaclesToSend[i].y);
    }
    msg.obstacles_number(target_number);
    msg.obstacles_x(obstacles_x);
    msg.obstacles_y(obstacles_y);
    mypub->publish(msg);
}
int main(int argc, char *argv[] ) {
    pid_t pid = getpid(); // Get the current process ID
    logger log("./logs/obstacles_gen.log", pid); // Initialize logger for this process with a unique log file

    std::ofstream pidFile("/tmp/obst.pid");
    pidFile << pid;
    pidFile.close();

    // Register the signal handlers
    signal(SIGUSR1, handlePauseSignal);
    signal(SIGUSR2, handleResumeSignal);

    std::random_device rd;  // Non-deterministic random seed
    std::mt19937 gen(rd()); // Mersenne Twister RNG
    Point drone_position{DRONE_POS_X,DRONE_POS_Y};
    // Define a uniform real distribution between 0 and 1
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    // Initialization
    Point obstaclesToSend[obstacles_number];
    

    DDSPublisher<Obstacles,ObstaclesPubSubType>* mypub = new DDSPublisher<Obstacles,ObstaclesPubSubType>();
    mypub->init(OBSTACLES_TOPIC_NAME);
    mypub->waitSub();



    // Initialize the obstacles generator
    WindowBorders borders = WindowBorders{START_X,START_Y,WIDTH,HEIGHT};
    ObjectsGenerator obstacles_obj_gen{borders.startX, borders.startX + borders.width - 1, borders.startY, borders.startY + borders.height - 1, obstacles_number};
    
    // Collect except points (drone's position and targets)
    std::vector<Point> except_points{drone_position};
    
        
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
        pubObstacle(mypub,obstaclesToSend);
        // Sleep for a while before next update
        usleep(500 * UPDATE_TIME);


        // Adjust drone position (round to nearest integer)
        // drone_position.x = round(worldState.getDronePos().x);
        // drone_position.y = round(worldState.getDronePos().y);

        // // Rebuild the except points list and generate new obstacles
        // except_points.clear();
        // except_points.push_back(drone_position);
        // for (const Point& target : worldState.targets_positions) {
        //     except_points.push_back(target);
        // }

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
    delete mypub;
    return 0;
}
