#include <sys/stat.h>
#include <fcntl.h>
#include <array>
#include "point.hpp"
#include "dynamics.hpp"
#include "world_state.hpp"
#include "objects_generator.hpp"
#include <algorithm>
#include <unistd.h>
#include "configs.hpp"
#include <map>
#include "logger.hpp"
#include <signal.h>   // For kill() and signal constants
#include <sys/types.h> // For pid_t

#include <fstream>
#include <csignal>
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

void printCmd(Command cmd){
 std::map<Command,std::string> cmdMap = {
        {Command::w,"w"},
        {Command::e,"e"},
        {Command::r,"r"},
        {Command::s,"s"},
        {Command::d,"d"},
        {Command::f,"f"},
        {Command::x,"x"},
        {Command::c,"c"},
        {Command::v,"v"},
        {Command::UNKNOWN,"UNKNOWN"}
    };

}


int main()
{
     pid_t pid = getpid();  // Get the process ID
    logger log("./logs/dybanics_server.log", pid); // Initialize logger for this process with a unique log file


    std::ofstream pidFile("/tmp/dynamic.pid");
    pidFile << pid;
    pidFile.close();

    // Register the signal handlers
    signal(SIGUSR1, handlePauseSignal);
    signal(SIGUSR2, handleResumeSignal);

    fd_set r_fds, w_fds;
    // mkfifo(dynamics_to_board_pipe, 0666);
    // int dynamics_to_board_fd = open(dynamics_to_board_pipe, O_WRONLY|O_CREAT|O_TRUNC,0666);

    // mkfifo(board_to_dynamics_pipe, 0666);
    // int board_to_dynamics_fd = open(board_to_dynamics_pipe, O_RDONLY|O_CREAT|O_TRUNC,0666);

    char *fd_str = getenv("dynamics_to_board_fd_write");
    if (fd_str == NULL) {
        fprintf(stderr, "Environment variable dynamics_to_board_fd_write not found\n");
        exit(1);
    }

    // Convert fd_str to an integer
    int dynamics_to_board_fd_write = atoi(fd_str);

    fd_str = getenv("board_to_dynamics_fd_read");
    if (fd_str == NULL) {
        fprintf(stderr, "Environment variable board_to_dynamics_fd_read not found\n");
        exit(1);
    }


    // Convert fd_str to an integer
    int board_to_dynamics_fd_read = atoi(fd_str);
    // initializations
    std::cout << board_to_dynamics_fd_read << std::endl;
    std::cout << dynamics_to_board_fd_write << std::endl;
    Point drone_position{5.0,5.0};
    WorldState<obstacles_number,target_number> worldState{drone_position};
    Dynamics dynamics{};
    // Command cmd{Command::f};
    WindowBorders borders{0,0,100,10};
    std::vector<Point> all_obstacles{};
    std::vector<Point> obstacles(obstacles_number);
    

    worldState.setBorder(borders);
    read(board_to_dynamics_fd_read,&worldState,sizeof(worldState));
    borders = worldState.getBorder();
    Point positions_hist[3] = {worldState.drone_position,worldState.drone_position,worldState.drone_position};
    ObjectsGenerator obstacles_obj_gen{borders.startX,borders.startX+borders.width-1,borders.startY,borders.startY+borders.height-1,obstacles_number};
    // get all obstacle points
    std::vector<Point> geo_fence_obstacles = obstacles_obj_gen.genrateGeoFenceObjects();

    int i =0;
    for(const Point& point:worldState.obstacles_positions){
            all_obstacles.push_back(point);
    }
    all_obstacles.insert(all_obstacles.end(), geo_fence_obstacles.begin(), geo_fence_obstacles.end());

    Point force;
    
    while(true){
          // Log heartbeat to indicate the process is still active
        log.logHeartbeat();  // Ensure the watchdog is aware this process is alive
        while (shouldPause.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait until resume signal is received
        }
        // update force
        force = dynamics.calcForce(positions_hist,all_obstacles,worldState.cmd);
        printCmd(worldState.cmd);
        // update the position of the drone
        if(worldState.cmd==Command::d){
            positions_hist[0] = drone_position;
            positions_hist[1] = drone_position;
            positions_hist[2] = drone_position;
        }
        else{
            drone_position = dynamics.updatePos(positions_hist,force);
            positions_hist[0] = positions_hist[1];
            positions_hist[1] = positions_hist[2];
            positions_hist[2] = drone_position;
        }
            
        


        // selectPipes(r_fds,w_fds,std::vector<int>{board_to_dynamics_fd}
        // ,std::vector<int>{dynamics_to_board_fd});

        // if (FD_ISSET(dynamics_to_board_fd,&w_fds))

        write(dynamics_to_board_fd_write,&drone_position,sizeof(drone_position));
        usleep(UPDATE_TIME);
        // update info 
        int k = read(board_to_dynamics_fd_read,&worldState,sizeof(worldState));
        if(k==0)
            continue;
        // update obstacles info
        i = 0;
        for(const Point& point:worldState.obstacles_positions){
            all_obstacles[i] = point;
            i++;
        }
        // update drone position
        // positions_hist[2] = worldState.drone_position;

        
        
    }
    close(dynamics_to_board_fd_write);
    close(board_to_dynamics_fd_read);
    return 0;
}