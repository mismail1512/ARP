#pragma once
#include <point.hpp>
#include <commands.hpp>
#include <vector>
#include <map>





class Dynamics
{
    private:
       double max_v = 500;
        double max_a = 1000;
        double F = 1.0;
        double T = 10.0*pow(10,-3); // in ms
        double K = 1.0; // viscous coffecient
        double M = 1.0; // kg
        // repulsion coffecients
        double p = 5.0;
        double n = 2.0;
        
    

////////////////////////////////////////////

  /*  double max_v;
    double max_a;
    double F;
    double T;
    double K;
    double M;
    double p;
    double n;
*/

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