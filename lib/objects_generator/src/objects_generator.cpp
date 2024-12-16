#include"objects_generator.hpp"
#include <random>
#include <algorithm>

Point ObjectsGenerator::generateRandomPoint(const std::vector<Point>& avoid_points) 
{
    static std::random_device rd; // Seed for random number generator
    static std::mt19937 gen(rd()); // Mersenne Twister random number engine
    std::uniform_int_distribution<> disX(x_min_+1, x_max_-1);
    std::uniform_int_distribution<> disY(y_min_+1, y_max_-1);

    Point new_point;
    do {
        new_point.x = disX(gen);
        new_point.y = disY(gen);
    } while (std::find(avoid_points.begin(), avoid_points.end(), new_point) != avoid_points.end());

    return new_point;
}

ObjectsGenerator::ObjectsGenerator(int x_min,int x_max,int y_min,int y_max,int points_number)
{
    x_min_ = x_min;
    x_max_ = x_max;
    y_min_ = y_min;
    y_max_ = y_max;
    points_number_ = points_number;
}

std::vector<Point> ObjectsGenerator::genrateObjects(std::vector<Point> avoid_points)
{
    std::vector<Point> points;
    for(int i=0;i<points_number_;i++)
    {
        Point point = generateRandomPoint(avoid_points);
        // Make the point not possible to be generated again
        avoid_points.push_back(point);
        points.push_back(point);
    }
    return points;
}

std::vector<Point> ObjectsGenerator::genrateGeoFenceObjects()
{
    std::vector<Point> geoFenceObjs{};
    Point p1;
    Point p2;
    for(double i=x_min_;i<x_max_;i++)
    {
        p1 ={i,0};
        p2 = {i,static_cast<double>(y_max_)};
        geoFenceObjs.push_back(p1);
        geoFenceObjs.push_back(p2);
    }

    for(double i=y_min_;i<y_max_;i++)
    {
        p1= {0,i};

        p2 = {static_cast<double>(x_max_),i};
        geoFenceObjs.push_back(p1);
        geoFenceObjs.push_back(p2);
    }
    return geoFenceObjs;

}


