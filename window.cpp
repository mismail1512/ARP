#include <unistd.h>
#include "visualizer.hpp"
#include <ncurses.h>
#include "world_state.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include "configs.hpp"
#include "logger.hpp"
#include <iostream>
#include <csignal>
#include <atomic>
#include <chrono>
#include <thread>
#include <fstream>


int main(int argc, char* argv[]) {
//std::cout << "Window process running. PID: " << pid << std::endl;
pid_t pid = getpid();

std::ofstream pidFile("/tmp/window_process.pid");
    pidFile << pid;
    pidFile.close();
    std::cout << "Window process running. PID: " << pid << std::endl;
    logger log("./logs/window.log", pid); // Initialize logger for this process with a unique log file



    mkfifo(windowPipe, 0666);
    int windowfd = open(windowPipe, O_RDONLY | O_CREAT | O_TRUNC, 0666);
    Point drone_pos{0, 0};
    WorldState<obstacles_number, target_number> worldState{drone_pos};
    read(windowfd, &worldState, sizeof(worldState));

    Visualizer vis{worldState.getBorder()};
    WINDOW* win = vis.generateWindow();

    std::vector<Point> tars(target_number);
    std::vector<Point> obs(obstacles_number);
    int i;

    while (true) {
        

        // Log heartbeat after sending targets
        log.logHeartbeat();
        i = 0;
        for (const Point& point : worldState.targets_positions) {
            tars[i] = point;
            i++;
        }
        i = 0;
        for (const Point& point : worldState.obstacles_positions) {
            obs[i] = point;
            i++;
        }

        // Draw the data
        drone_pos = worldState.getDronePos();
        drone_pos.x = round(drone_pos.x);
        drone_pos.y = round(drone_pos.y);
        vis.draw(win, drone_pos, tars, obs);
        // Display the score below the border
        wattron(win, COLOR_PAIR(1)); // Enable yellow color
        mvwprintw(win, worldState.getBorder().startY , 1, "Score: %d ", worldState.score);
        wattroff(win, COLOR_PAIR(1)); // Disable yellow color
        vis.refresh(win);

        usleep(UPDATE_TIME);
        // Read data from the board
        read(windowfd, &worldState, sizeof(worldState));
    }

    delwin(win);
    endwin();
    close(windowfd);
    return 0;
}
