#pragma once
#include<vector>
#include"point.hpp"


class ObjectsGenerator
{
    private:
        int x_min_, x_max_, y_min_, y_max_,points_number_;
        // Function to generate random points
        Point generateRandomPoint(const std::vector<Point>& avoid_points);
    public:
        ObjectsGenerator(int x_min,int x_max,int y_min,int y_max,int points_number_);
        std::vector<Point> genrateObjects(std::vector<Point> avoid_points);
        std::vector<Point> genrateGeoFenceObjects();

};