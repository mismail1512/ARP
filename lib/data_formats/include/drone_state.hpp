#pragma once

#include "point.hpp"
#include "commands.hpp"
#include <vector>

struct DroneState 
{
    Point drone_position;
    std::vector<Point> nearby_obstacles_positions;
    std::vector<Command> commands;
};