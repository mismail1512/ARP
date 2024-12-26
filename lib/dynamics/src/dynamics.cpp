#include "dynamics.hpp"
#include<math.h>
#include <iostream>
#include <map>


Dynamics::Dynamics()
{

}


Dynamics::Dynamics(const std::map<std::string, float>& params) {
    try {
        // Use the values from the params map
        max_v = params.at("max_velocity");
        max_a = params.at("max_acceleration");
        F = params.at("force_step");
        T = params.at("time_step");
        K = params.at("viscous_coefficient");
        M = params.at("mass");
        p = params.at("repulsion_distance");
        n = params.at("repulsion_factor");
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Missing parameter in config map: " << e.what() << std::endl;
    }
}

void Dynamics::refresh() {

    std::map<std::string, float> params = params_manager1.getConfigAsMap();

    try {
        std::cout <<"refresh"<<std::endl;
        // Use the values from the params map
        max_v = params.at("max_velocity");
        std::cout <<"max_v : "<<max_v<<std::endl;

        max_a = params.at("max_acceleration");
        F = params.at("force_step");
        T = params.at("time_step");
        K = params.at("viscous_coefficient");
        M = params.at("mass");
        p = params.at("repulsion_distance");
        n = params.at("repulsion_factor");
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Missing parameter in config map: " << e.what() << std::endl;
    }
}

Point Dynamics::calcInertialForce(const Point positions[3])
{
    // std::cout <<"first pos " << positions[0] << " 2nd " << positions[1] << " 3rd " << positions[2] << " " << ((positions[0]+positions[2]-positions[1]*2)*M)/pow(T,2) << " " <<  ((positions[2]-positions[1])*K)/T<< std::endl;
    
    Point viscousity_part = ((positions[2]-positions[1])*-K)/T;
    
    return ((positions[0]+positions[2]-positions[1]*2)*M)/pow(T,2)+viscousity_part;
}


Point Dynamics::calcRepulsionForce(const Point& position,const std::vector<Point>& obstacles_pos)
{
    Point force{0.0,0.0};
    double d_q = 0.0;
    for(const Point& pos:obstacles_pos)
    {
        d_q = pos.dist(position);
        if (d_q > p)
            continue;
        Point force_calc=n*(1/d_q-1/p)*(1/pow(d_q,2))*((position-pos)/d_q);
        // std::cout << "rep" << force_calc << " " << pos << " "<< position <<std::endl;
        force = force + force_calc;
    }
    // std::cout << "repulsion force" << force << std::endl;
    return force;
}

Point Dynamics::calcCommandForce(const Command& command)
{
    Point force{0.0,0.0};
    switch (command)
    {
    case Command::w:
        force.x = -F; // Top left diagonal
        force.y = -F;
        break;
    case Command::e:  // Top-center
        force.x = 0;
        force.y = -F;
        break;
    case Command::r:  // Top-right diagonal
        force.x = F;
        force.y = -F;
        break;
    case Command::s:  // Middle-left
        force.x = -F;
        force.y = 0;
        break;
    case Command::d:  // No force (center)
        force.x = 0;
        force.y = 0;
        break;
    case Command::f:  // Middle-right
        force.x = F;
        force.y = 0;
        break;
    case Command::x:  // Bottom-left diagonal
        force.x = -F;
        force.y = F;
        break;
    case Command::c:  // Bottom-center
        force.x = 0;
        force.y = F;
        break;
    case Command::v:  // Bottom-right diagonal
        force.x = F;
        force.y = F;
        break;
    
    default:
        break;
    }

    return force;
}

Point Dynamics::calcForce(const Point positions[3] ,const std::vector<Point>& obstacles_pos,const Command& command)
{
    Point force{0,0};
    force = force+calcCommandForce(command);
    force = force + ((positions[2]-positions[1])*-K)/T;
    std::cout << " force because of viscosuity " << ((positions[2]-positions[1])*-K)/T << std::endl;
    // force = force+calcInertialForce(positions);
    force = force+calcRepulsionForce(positions[2],obstacles_pos);
    // std::cout << "force after repulsion" << force << std::endl;
    return force;
}

Point Dynamics::calcForce(const Point positions[3],const std::vector<Point>& obstacles_pos)
{
    Point force{0,0};
    // force = force+calcInertialForce(positions);
    force = force + ((positions[2]-positions[1])*-K)/T;
    std::cout << " force because of viscosuity " << ((positions[2]-positions[1])*-K)/T << std::endl;

    force = force+calcRepulsionForce(positions[2],obstacles_pos);
    // std::cout << "force after repulsion" << force << std::endl;
    return force;
}

Point Dynamics::updatePos(const Point positions[3],const Point& force)
{
    Point a = force/M;
    a.x = fmax(fmin(a.x,max_a*T),-max_a*T);
    a.y = fmax(fmin(a.y,max_a*T),-max_a*T);
    Point v = (positions[2]-positions[1])/T+ a*T;
    v.x = fmax(fmin(v.x,max_v*T),-max_v*T);
    v.y = fmax(fmin(v.y,max_v*T),-max_v*T);
    std::cout << "a " << a <<std::endl;
    std::cout << "v " << v <<std::endl;
    std::cout << "0 " << positions[0] << std::endl;
    std::cout << "1 " << positions[1] << std::endl;
    std::cout << "2 " << positions[2] << std::endl;


    Point translation = v*T;
    return positions[2] + translation;
}

