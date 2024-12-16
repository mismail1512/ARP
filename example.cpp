#include "object_order.hpp"
#include "point.hpp"
#include"world_state.hpp"
#include "commands.hpp"
#include "objects_generator.hpp"
#include<array>
#include "dynamics.hpp"
#include "visualizer.hpp"
#include <algorithm>
#include <unistd.h>
Point x;




int main()
{

    // initializations
    WindowBorders borders{0,0,100,10};
    Point drone_position{5,5};
    Point temp_drone_position{5,5};
    int target_number = 2;
    int obstacles_number = 20;
    Dynamics dynamics{};
    Command cmd{Command::f};

    ObjectsGenerator targets_obj_gen{borders.startX,borders.startX+borders.width-1,borders.startY,borders.startY+borders.height-1,target_number};
    ObjectsGenerator obstacles_obj_gen{borders.startX,borders.startX+borders.width-1,borders.startY,borders.startY+borders.height-1,obstacles_number};
    // generate targets
    std::vector<Point> except_points{drone_position};

    std::vector<Point> targets = targets_obj_gen.genrateObjects(except_points);
    for(const Point& target:targets)
        except_points.push_back(target);
    // generate obstacles
    std::vector<Point> obstacles = obstacles_obj_gen.genrateObjects(except_points);

    Visualizer vis{borders};
    WINDOW* win = vis.generateWindow();
    std::cout << "Terminal size: " << LINES << "x" << COLS << std::endl;

    // get all obstacle points
    std::vector<Point> geo_fence_obstacles = obstacles_obj_gen.genrateGeoFenceObjects();
    
    std::vector<Point> all_obstacles{};
    all_obstacles.insert(all_obstacles.end(), obstacles.begin(), obstacles.end());
    all_obstacles.insert(all_obstacles.end(), geo_fence_obstacles.begin(), geo_fence_obstacles.end());
    for(Point p:all_obstacles)
        std::cout<<p<<std::endl;
    std::array<Point,3> positions{drone_position,drone_position,drone_position};
    std::array<Point,3> temp_positions{drone_position,drone_position,drone_position};
    Point force = dynamics.calcForce(positions,all_obstacles,cmd);
    fflush(stdout);

    while(true){

        temp_positions = dynamics.updatePos(positions,force);
        // check if there are any obstacles in the way
        temp_drone_position = temp_positions[2];
        temp_drone_position.x = round(temp_drone_position.x);
        temp_drone_position.y = round(temp_drone_position.y);
        // std::cout << "temp" << temp_drone_position<<std::endl;
        if(std::find(all_obstacles.begin(), all_obstacles.end(), temp_drone_position) == all_obstacles.end())
            // If the new position is not in an obstacle place update
            positions = temp_positions;
        else
        {
            positions[0] = positions[1];
            positions[1] = positions[2];
        }

        
        drone_position = positions[2];
        // std::cout << force << " " << drone_position << std::endl;
        fflush(stdout);
        vis.draw(win,drone_position,targets,obstacles);
        vis.refresh(win);
        usleep(10000);
        force = dynamics.calcForce(positions,all_obstacles,cmd);
    }
    delwin(win);
    endwin();
    return 0;
}