#include <vector>
#include <fcntl.h>

#define TARGETS_TOPIC_NAME "Targets"
#define OBSTACLES_TOPIC_NAME "Obstacles"


#define windowPipe "/tmp/window"
#define dynamics_to_board_pipe "/tmp/dynamics_to_board"
#define board_to_dynamics_pipe "/tmp/board_to_dynamics"
#define input_to_board_pipe "/tmp/input_to_board"

#define UPDATE_TIME 10000


constexpr int START_X = 0;
constexpr int START_Y = 0;
constexpr int WIDTH = 10;
constexpr int HEIGHT = 10;
constexpr double DRONE_POS_X = 5.0;
constexpr double DRONE_POS_Y = 5.0;


constexpr int target_number = 3;
constexpr int obstacles_number = 3;

constexpr float w1 = 100;
constexpr float w2 = 0.1; // -1 every 10 seconds
constexpr float w3 = 0.1; // -1 after distance 10

void selectPipes(fd_set& r_fds,fd_set& w_fds,const std::vector<int>& readPipes,const std::vector<int>& writePipes){
    
    timeval time_out;
    FD_ZERO(&r_fds);
    for(const int& pipe:readPipes)
        FD_SET(pipe, &r_fds);

    FD_ZERO(&w_fds);
    for(const int& pipe:writePipes)
        FD_SET(pipe, &w_fds);

    time_out.tv_sec = 0;
    time_out.tv_usec = 1000;

    int selec_ret = select(100,&r_fds,&w_fds,NULL,&time_out);
    if(selec_ret==-1)
    {
        perror("select error\n");
        return;
    }
    else if (selec_ret == 0) {
        std::cerr << "select timeout, no data available" << std::endl;
        return; // Avoid processing on timeout
    }
}

