#pragma once
#include<vector>
#include"point.hpp"
#include"world_state.hpp"
#include <ncurses.h>




class Visualizer
{
    public:
        Visualizer(const WindowBorders& rect);
        ~Visualizer();

        void draw(WINDOW* win,const Point& dronePosition,const std::vector<Point>& targets,const std::vector<Point>& obstacles);
        //void updateBorders(const WindowBorders& rect);
        void drawBorders(WINDOW* win);
        void drawDrone(WINDOW* win,const Point& dronePosition);
        void drawTargets(WINDOW* win,const std::vector<Point>& targets);
        void drawObstacles(WINDOW* win,const std::vector<Point>& obstacles);
        WINDOW*  generateWindow();
        void refresh(WINDOW* win);
    private:
        WindowBorders borders;
        Point prevDrone{-1,-1};
        std::vector<Point>previousObstacles{};
        std::vector<Point>previousTargets{};
        void initializeNcurses();
        
};