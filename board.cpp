#include "point.hpp"
#include "commands.hpp"
#include "objects_generator.hpp"
#include <array>
#include <algorithm>
#include <unistd.h>
#include "world_state.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include "configs.hpp"
#include "logger.hpp"  // Include the logger header
#include <signal.h>    // For kill() and signal constants
#include <sys/types.h> // For pid_t

#include <csignal>
#include <atomic>
#include <chrono>
#include <thread>
#include <fstream>
Point x;

// Atomic flag to indicate if the process should pause
std::atomic<bool> shouldPause(false);
bool reset = false;

void handlePauseResumeSignal(int signal)
{
    if (signal == SIGUSR1)
    { // 'p'
        // Toggle the pause state
        bool current = shouldPause.load();
        shouldPause.store(!current); // Flip the state
    }
}

void handleResetSignal(int signal)
{
    if (signal == SIGUSR2)
    { // 'st'
        std::cout << " reset =true;.." << std::endl;
        reset = true;
    }
}

int calcScore(int targets, double time, float distance)
{
    float targetsRatio = static_cast<float>(target_number - targets) / target_number;
    return w1 * targetsRatio - w2 * time - w3 * distance;
}
int main()
{
    double time = 0.0;
    float distance = 0.0;
    pid_t pid = getpid();                // Get the process ID
    logger log("./logs/board.log", pid); // Initialize logger for this process with a unique log file

    std::ofstream pidFile("/tmp/board.pid");
    pidFile << pid;
    pidFile.close();

    // Register the signal handlers
    signal(SIGUSR1, handlePauseResumeSignal);
    signal(SIGUSR2, handleResetSignal);

    // signal(SIGUSR2, handleResumeSignal);

    fd_set r_fds, w_fds;
    mkfifo(windowPipe, 0666);
    int windowfd = open(windowPipe, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    mkfifo(obstacles_to_board_pipe, 0666);
    int obstacles_to_board_pipe_fd = open(obstacles_to_board_pipe, O_RDONLY | O_CREAT | O_TRUNC, 0666);

    mkfifo(board_to_obstacles_pipe, 0666);
    int board_to_obstacles_fd = open(board_to_obstacles_pipe, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    mkfifo(targets_to_board_pipe, 0666);
    int targets_to_board_fd = open(targets_to_board_pipe, O_RDONLY | O_CREAT | O_TRUNC, 0666);

    mkfifo(board_to_targets_pipe, 0666);
    int board_to_targets_fd = open(board_to_targets_pipe, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    int fd1[2];
    if (pipe(fd1) == -1)
    {
        perror("pipe failed");
        exit(1);
    }
    int fd2[2];
    if (pipe(fd2) == -1)
    {
        perror("pipe failed");
        exit(1);
    }
    int dynamics_to_board_fd_read = fd1[0];
    int dynamics_to_board_fd_write = fd1[1];
    int board_to_dynamics_fd_read = fd2[0];
    int board_to_dynamics_fd_write = fd2[1];

    pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        exit(1);
    }
    else if (pid == 0)
    {
        close(dynamics_to_board_fd_read);
        close(board_to_dynamics_fd_write);
        // Convert fd[0] to a string and set it as an environment variable

        char fd0_str[10];
        char fd1_str[10];
        snprintf(fd0_str, sizeof(fd0_str), "%d", dynamics_to_board_fd_write);
        snprintf(fd1_str, sizeof(fd1_str), "%d", board_to_dynamics_fd_read);
        setenv("dynamics_to_board_fd_write", fd0_str, 1); // Set MY_FD in the environment
        setenv("board_to_dynamics_fd_read", fd1_str, 1);
        // Execute process_P
        execl("./build/dynamics_server", "dynamics_server", NULL);
        // If exec fails
        perror("exec failed");
        exit(1);
    }
    close(board_to_dynamics_fd_read);
    close(dynamics_to_board_fd_write);
    // mkfifo(dynamics_to_board_pipe, 0666);
    // int dynamics_to_board_fd = open(dynamics_to_board_pipe, O_RDONLY|O_CREAT|O_TRUNC,0666);

    // mkfifo(board_to_dynamics_pipe, 0666);
    // int board_to_dynamics_fd = open(board_to_dynamics_pipe, O_WRONLY|O_CREAT|O_TRUNC,0666);

    mkfifo(input_to_board_pipe, 0666);
    int input_to_board_pipe_fd = open(input_to_board_pipe, O_RDONLY | O_CREAT | O_TRUNC, 0666);

    // initializations
    WindowBorders borders{0, 0, BORDER_WIDTH, BORDER_HEIGHT};
    Point drone_position{DRONE_X, DRONE_Y};
    Point temp_drone_position{DRONE_X, DRONE_Y};
    Command cmd{Command::f};
    std::vector<Point> all_obstacles{};
    int i = 0;

    WorldState<obstacles_number, target_number> worldState{drone_position};
    WorldState<obstacles_number, target_number> tempWorldState{drone_position};

    worldState.setBorder(borders);

    Point default_target_pos[target_number]{};

    // tell targets generator about the world
    write(board_to_targets_fd, &worldState, sizeof(worldState));
    // generate targets
    read(targets_to_board_fd, &worldState.targets_positions, sizeof(worldState.targets_positions));
    for (int i = 0; i < target_number; i++)
        default_target_pos[i] = worldState.targets_positions[i];
    // tell obstacles geenrator about the world
    write(board_to_obstacles_fd, &worldState, sizeof(worldState));
    // generate obstacles
    read(obstacles_to_board_pipe_fd, &worldState.obstacles_positions, sizeof(worldState.obstacles_positions));

    write(board_to_dynamics_fd_write, &worldState, sizeof(worldState));

    ObjectsGenerator obstacles_obj_gen{borders.startX, borders.startX + borders.width - 1, borders.startY, borders.startY + borders.height - 1, obstacles_number};
    // get all obstacle points
    std::vector<Point> geo_fence_obstacles = obstacles_obj_gen.genrateGeoFenceObjects();
    i = 0;
    for (const Point &point : worldState.obstacles_positions)
    {
        all_obstacles.push_back(point);
    }
    all_obstacles.insert(all_obstacles.end(), geo_fence_obstacles.begin(), geo_fence_obstacles.end());
    bool wroteToDynamics = false;
    tempWorldState = worldState;
    int targetsNumber = 0;
    int currentTargetIdx = 0;
    while (true)
    {

        // handling signals and pausing
        signal(SIGUSR2, handleResetSignal);
        while (shouldPause.load())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Wait until resume signal is received
        }

        // Log heartbeat to indicate that the process is still active
        log.logHeartbeat(); 

        
        selectPipes(r_fds, w_fds, std::vector<int>{obstacles_to_board_pipe_fd, targets_to_board_fd, dynamics_to_board_fd_read, input_to_board_pipe_fd}, std::vector<int>{windowfd, board_to_obstacles_fd, board_to_targets_fd, board_to_dynamics_fd_write});

        // update all the processes with current state
        if (FD_ISSET(windowfd, &w_fds))
            write(windowfd, &worldState, sizeof(worldState));
        if (FD_ISSET(board_to_obstacles_fd, &w_fds))
            write(board_to_obstacles_fd, &worldState, sizeof(worldState));
        if (FD_ISSET(board_to_targets_fd, &w_fds))
            write(board_to_targets_fd, &worldState, sizeof(worldState));
        
        tempWorldState = worldState;
        usleep(UPDATE_TIME);

        // considering simulation time and assuming processing time is negligible
        time += static_cast<double>(UPDATE_TIME * pow(10, -6));
        
        // reading new info about obstacles and input
        if (FD_ISSET(input_to_board_pipe_fd, &r_fds))
            read(input_to_board_pipe_fd, &worldState.cmd, sizeof(worldState.cmd));
        else
            worldState.cmd = Command::UNKNOWN;

        if (FD_ISSET(obstacles_to_board_pipe_fd, &r_fds))
            read(obstacles_to_board_pipe_fd, &worldState.obstacles_positions, sizeof(worldState.obstacles_positions));
        
        // updating position section
        if (FD_ISSET(board_to_dynamics_fd_write, &w_fds))
        {
            write(board_to_dynamics_fd_write, &worldState, sizeof(worldState));
            wroteToDynamics = true;
        }

        if (FD_ISSET(dynamics_to_board_fd_read, &r_fds) && wroteToDynamics)
        {
            wroteToDynamics = false;
            read(dynamics_to_board_fd_read, &drone_position, sizeof(drone_position));
        }
        if(FD_ISSET(targets_to_board_fd, &r_fds))
            read(targets_to_board_fd, &default_target_pos, sizeof(worldState.targets_positions));
        // reject big jumps
        if(drone_position.dist(worldState.drone_position) > 2)
            drone_position = worldState.drone_position;
        // Check if the drone is on the current target
        // check to make sure that we don't run into an obstacle
        temp_drone_position.x = round(drone_position.x);
        temp_drone_position.y = round(drone_position.y);
        worldState.drone_position = drone_position;

        // update all obstacles
        i = 0;
        for (const Point &point : worldState.obstacles_positions)
        {
            all_obstacles[i] = point;
            i++;
        }
        // check if we ran into an obstacle
        if (std::find(all_obstacles.begin(), all_obstacles.end(), temp_drone_position) != all_obstacles.end() || temp_drone_position.x > borders.startX + borders.width || temp_drone_position.y > borders.startY + borders.height + 1)
        {
            // obstacle in the same place as the drone
            // invalid state
            // fall into the previous state
            worldState = tempWorldState;
        }
        else
        {
            // update distance
            distance += tempWorldState.drone_position.dist(worldState.drone_position);
            // handle running into the current target
            if (temp_drone_position == worldState.targets_positions[currentTargetIdx]) {
                // Remove the target    
                worldState.targets_positions[currentTargetIdx] = {NAN, NAN};
                currentTargetIdx++;
            }
        }
        

        // handle reset
        if (reset)
        {
            drone_position = {DRONE_X, DRONE_Y};
            worldState.drone_position = drone_position;
            worldState.score = 0;
            time = 0;
            distance = 0;

            // Reinitialize WorldState
            worldState.drone_position = drone_position;
            tempWorldState.drone_position = drone_position;
            read(targets_to_board_fd, &worldState.targets_positions, sizeof(worldState.targets_positions));
            currentTargetIdx = 0;
            reset = false;
        }
        // calculate targets number
        targetsNumber=0;
        for (const Point& point:worldState.targets_positions){

            if(!point.isNull()){
                targetsNumber++;
            }
        }
        worldState.score = calcScore(targetsNumber, time, distance);
    }

    close(windowfd);
    close(board_to_obstacles_fd);
    close(obstacles_to_board_pipe_fd);
    close(targets_to_board_fd);
    close(board_to_targets_fd);
    close(dynamics_to_board_fd_read);
    close(board_to_dynamics_fd_write);
    return 0;
}
