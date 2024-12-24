#include "point.hpp"
#include "commands.hpp"
#include "objects_generator.hpp"
#include  <array>
#include <algorithm>
#include <unistd.h>
#include "world_state.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include "configs.hpp"
#include "logger.hpp" // Include the logger header
#include <signal.h>   // For kill() and signal constants
#include <sys/types.h> // For pid_t

#include <csignal>
#include <atomic>
#include <chrono>
#include <thread>
#include <fstream>
Point x;

// Atomic flag to indicate if the process should pause
std::atomic<bool> shouldPause(false);

int reset= false;

void handleresetSignal(int signal) {
    if (signal == SIGUSR1) { 

        reset= true;
    }
}

int calcScore(int targets ,double time,float distance){
    float targetsRatio = static_cast<float>(target_number-targets)/target_number;
    return w1*targetsRatio-w2*time - w3*distance ;
}
int main()
{
    double time = 0.0;
    float distance = 0.0;
    pid_t pid = getpid();  // Get the process ID
   logger log("./logs/board.log", pid); // Initialize logger for this process with a unique log file


   std::ofstream pidFile("/tmp/board.pid");
    pidFile << pid;
    pidFile.close();

    // Register the signal handlers
    signal(SIGUSR1, handleresetSignal);
    //signal(SIGUSR2, handleResumeSignal);

    fd_set r_fds, w_fds;
    mkfifo(windowPipe, 0666);
    int windowfd = open(windowPipe, O_WRONLY|O_CREAT|O_TRUNC,0666);

    mkfifo(obstacles_to_board_pipe, 0666);
    int obstacles_to_board_pipe_fd = open(obstacles_to_board_pipe, O_RDONLY|O_CREAT|O_TRUNC,0666);

    mkfifo(board_to_obstacles_pipe, 0666);
    int board_to_obstacles_fd = open(board_to_obstacles_pipe, O_WRONLY|O_CREAT|O_TRUNC,0666);

    mkfifo(targets_to_board_pipe, 0666);
    int targets_to_board_fd = open(targets_to_board_pipe, O_RDONLY|O_CREAT|O_TRUNC,0666);

    mkfifo(board_to_targets_pipe, 0666);
    int board_to_targets_fd = open(board_to_targets_pipe, O_WRONLY|O_CREAT|O_TRUNC,0666);

    mkfifo(dynamics_to_board_pipe, 0666);
    int dynamics_to_board_fd = open(dynamics_to_board_pipe, O_RDONLY|O_CREAT|O_TRUNC,0666);

    mkfifo(board_to_dynamics_pipe, 0666);
    int board_to_dynamics_fd = open(board_to_dynamics_pipe, O_WRONLY|O_CREAT|O_TRUNC,0666);
    
    mkfifo(input_to_board_pipe, 0666);
    int input_to_board_pipe_fd = open(input_to_board_pipe, O_RDONLY|O_CREAT|O_TRUNC,0666);
    
    
    // initializations
    WindowBorders borders{0,0,10,10};
    Point drone_position{5.0,5.0};
    Point temp_drone_position{5.0,5.0};
    Command cmd{Command::f};
    std::vector<Point> all_obstacles{};
    int i = 0;

    WorldState<obstacles_number,target_number> worldState{drone_position};
    WorldState<obstacles_number,target_number> tempWorldState{drone_position};

    worldState.setBorder(borders);

    
    // tell targets generator about the world
    write(board_to_targets_fd,&worldState,sizeof(worldState));
    // generate targets
    read(targets_to_board_fd,&worldState.targets_positions,sizeof(worldState.targets_positions));
    // tell obstacles geenrator about the world
    write(board_to_obstacles_fd,&worldState,sizeof(worldState));
    // generate obstacles
    read(obstacles_to_board_pipe_fd,&worldState.obstacles_positions,sizeof(worldState.obstacles_positions));

    write(dynamics_to_board_fd,&worldState,sizeof(worldState));
    
    ObjectsGenerator obstacles_obj_gen{borders.startX,borders.startX+borders.width-1,borders.startY,borders.startY+borders.height-1,obstacles_number};
    // get all obstacle points
    std::vector<Point> geo_fence_obstacles = obstacles_obj_gen.genrateGeoFenceObjects();
    i =0;
    for(const Point& point:worldState.obstacles_positions){
        all_obstacles.push_back(point);
    }
    all_obstacles.insert(all_obstacles.end(), geo_fence_obstacles.begin(), geo_fence_obstacles.end());
    bool wroteToDynamics = false;
    tempWorldState = worldState;
    int targetsNumber=0;
    while(true){
        
        // Log heartbeat to indicate that the process is still active
        log.logHeartbeat();  // Log heartbeat at each iteration to ensure the watchdog monitors this process
       // while (shouldPause.load()) {
         //   std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait until resume signal is received
        //}
        
        selectPipes(r_fds,w_fds,std::vector<int>{obstacles_to_board_pipe_fd,targets_to_board_fd,dynamics_to_board_fd,input_to_board_pipe_fd}
        ,std::vector<int>{windowfd,board_to_obstacles_fd,board_to_targets_fd,board_to_dynamics_fd});

        // update all the processes with current state
        if(FD_ISSET(windowfd,&w_fds))
        {
            write(windowfd,&worldState,sizeof(worldState));
        }
            
        
            
        if(FD_ISSET(board_to_obstacles_fd,&w_fds))
            write(board_to_obstacles_fd,&worldState,sizeof(worldState));
        if(FD_ISSET(board_to_targets_fd,&w_fds))
            write(board_to_targets_fd,&worldState,sizeof(worldState));
        tempWorldState = worldState;
        usleep(UPDATE_TIME);
        // considering simulation time and assuming processing time is negligible
        time += static_cast<double>(UPDATE_TIME*pow(10,-6));
        // read new obstacles and targets
        if(FD_ISSET(input_to_board_pipe_fd,&r_fds))  
            read(input_to_board_pipe_fd,&worldState.cmd,sizeof(worldState.cmd));

        if (reset==true) 
            {
                drone_position = {1.0, 1.0};
                worldState.drone_position = drone_position;
                // Notify dynamics about the reset
                write(dynamics_to_board_fd, &worldState, sizeof(worldState));

                // Reinitialize obstacles and targets
                write(board_to_targets_fd, &worldState, sizeof(worldState));  // Notify targets generator

                read(targets_to_board_fd, &worldState.targets_positions, sizeof(worldState.targets_positions));


                write(board_to_obstacles_fd, &worldState, sizeof(worldState));  // Notify obstacles generator

                read(obstacles_to_board_pipe_fd, &worldState.obstacles_positions, sizeof(worldState.obstacles_positions));

                // Update all_obstacles to reflect the new world state
                all_obstacles.clear();
                all_obstacles.insert(all_obstacles.end(), std::begin(worldState.obstacles_positions), std::end(worldState.obstacles_positions));
                all_obstacles.insert(all_obstacles.end(), geo_fence_obstacles.begin(), geo_fence_obstacles.end());
                reset= false;

            }

        
            
        if(FD_ISSET(obstacles_to_board_pipe_fd,&r_fds))
            read(obstacles_to_board_pipe_fd,&worldState.obstacles_positions,sizeof(worldState.obstacles_positions));
        if(FD_ISSET(targets_to_board_fd,&r_fds))
            read(targets_to_board_fd,&worldState.targets_positions,sizeof(worldState.targets_positions));


        if (FD_ISSET(board_to_dynamics_fd,&w_fds))
        {
            write(board_to_dynamics_fd,&worldState,sizeof(worldState));
            wroteToDynamics = true;
        }


        if(FD_ISSET(dynamics_to_board_fd,&r_fds) && wroteToDynamics)
        {
            wroteToDynamics = false;
            read(dynamics_to_board_fd,&drone_position,sizeof(drone_position));
            // std::cout << "reading drone pos " << drone_position << std::endl;
        }

        worldState.drone_position = drone_position;

            
    
        // update all obstacles
        i =0;
        for(const Point& point:worldState.obstacles_positions){
            all_obstacles[i] = point;
            i++;
        }   
        // check to make sure that we don't run into an obstacle
        temp_drone_position.x = round(drone_position.x);
        temp_drone_position.y = round(drone_position.y);

            
        if(std::find(all_obstacles.begin(), all_obstacles.end(), temp_drone_position) != all_obstacles.end())
        {
            // obstacle in the same place as the drone
            // invalid state
            // fall into the previous state
            worldState = tempWorldState;
        } 
        else{
            distance += tempWorldState.drone_position.dist(worldState.drone_position);
        }     
        targetsNumber=0;
        
        for (const Point& point:worldState.targets_positions){
            if(!point.isNull()){
                targetsNumber++;
            }
                
        }
            
        worldState.score = calcScore(targetsNumber,time,distance); 
        
    }
    close(windowfd);
    close(board_to_obstacles_fd);
    close(obstacles_to_board_pipe_fd);
    close(targets_to_board_fd);
    close(board_to_targets_fd);
    close(dynamics_to_board_fd);
    close(board_to_dynamics_fd);
    return 0;
}


