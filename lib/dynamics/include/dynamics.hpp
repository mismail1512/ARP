#pragma once
#include <point.hpp>
#include <commands.hpp>
#include <vector>
#include <map>





class Dynamics
{
    private:
       double max_v = 500000;
        double max_a = 1000000;
        double F = 1.0;
        double T = 100000.0*pow(10,-6);
        double K = 0.01; // viscous coffecient
        double M = 1.0; // kg
        // repulsion coffecients
        double p = 3.0;
        double n = 0.5;

        Point calcInertialForce(const Point positions[3]); // [p0,p1,p2]
        Point calcRepulsionForce(const Point& position,const std::vector<Point>& obstacle_pos);
        Point calcCommandForce(const Command& command);

    public:
        Dynamics();
        Dynamics(const std::map<std::string, float>& params);  // Constructor

        Point calcForce(const Point positions[3],const std::vector<Point>& obstacles_pos,const Command& command);
        Point calcForce(const Point positions[3],const std::vector<Point>& obstacles_pos);
        Point updatePos(const Point positions[3],const Point& force);



};