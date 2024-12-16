#pragma once


#include "point.hpp"
#include "commands.hpp"
#include <vector>

struct WindowBorders {
    int startX, startY, width, height;
};
#pragma pack(1)
template <int T1, int T2>
struct WorldState 
{
    WorldState(Point drone_pos){
        for (int i = 0; i < T1; ++i) {
                obstacles_positions[i] = Point{0,0};
            }
        for (int i = 0; i < T2; ++i) {
                targets_positions[i] = Point{0,0};
        }
        drone_position = drone_pos;

    }
    WorldState(Point drone, Point obstacles[T1], Point targets[T2], WindowBorders border) {
            for (int i = 0; i < T1; ++i) {
                obstacles_positions[i] = obstacles[i];
            }
            for (int i = 0; i < T2; ++i) {
                targets_positions[i] = targets[i];
            }
            drone_position = drone;

            setBorder(border);

        }
    Point getDronePos(){
        return drone_position;
    }
    WindowBorders getBorder(){
        return WindowBorders{startX, startY, width, height};
    }
    void setBorder(WindowBorders border){
        startX = border.startX;
        startY = border.startY;
        height = border.height;
        width = border.width;
    }


    
    Point drone_position;
    Point obstacles_positions[T1];
    Point targets_positions[T2];
    Command cmd = Command::UNKNOWN;
    int startX, startY, width, height;
    
};
#pragma pack()
