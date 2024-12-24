#include <iostream>
#include "visualizer.hpp"



Visualizer::Visualizer(const WindowBorders& rect): borders(rect)
{
    
    initializeNcurses();
    
}
Visualizer::~Visualizer()
{

}
void Visualizer::initializeNcurses()
{
    initscr();            // Initialize ncurses mode
    start_color();        // Enable color functionality
    if (!has_colors()) {  // Check if terminal supports colors
        std::cerr << "Error: Terminal does not support colors.\n";
        exit(1);
    }
    curs_set(0);          // Hide the cursor
    noecho();             // Disable echoing of typed characters
    keypad(stdscr, TRUE); // Enable special keys
    timeout(-1);          // Wait indefinitely for user input

    // Define color pairs
    init_pair(1, COLOR_BLUE, COLOR_BLACK);   // Blue for the drone
    init_pair(2, COLOR_GREEN, COLOR_BLACK);  // Green for targets
    init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Orange for obstacles
    init_pair(4, COLOR_RED, COLOR_BLACK);    // Red for borders
    
    
}
WINDOW*  Visualizer::generateWindow()
{
    WINDOW* win = newwin(borders.height+1, borders.width+1, borders.startY, borders.startX);
    if (!win) {
        endwin();
        std::cerr << "Error: Failed to create window.\n";
    }
    return win;

}
void Visualizer::drawBorders(WINDOW* win)
{
    wattron(win, COLOR_PAIR(4)); // Use red color
    for (int x = 0; x < borders.width; ++x) {
        mvwaddch(win, 1, x, '#');                // Top border
        mvwaddch(win, borders.height , x, '#');  // Bottom border
    }
    for (int y = 1; y < borders.height+1; ++y) {
        mvwaddch(win, y, 0, '#');                // Left border
        mvwaddch(win, y, borders.width , '#');   // Right border
    }
    wattroff(win, COLOR_PAIR(4));
}



void Visualizer::drawDrone(WINDOW* win,const Point& dronePosition) {
    if (prevDrone.x != -1 && prevDrone.y != -1) {
        mvwaddch(win, prevDrone.y+1, prevDrone.x, ' '); // Erase old position
    }
    wattron(win, COLOR_PAIR(1)); // Use blue color
    mvwaddch(win, static_cast<int>(dronePosition.y+1), static_cast<int>(dronePosition.x), '+');
    wattroff(win, COLOR_PAIR(1));
    prevDrone = dronePosition;
}


void Visualizer::drawTargets(WINDOW* win,const std::vector<Point>& targets) {
    wattron(win, COLOR_PAIR(2)); // Use green color
    for (const auto& target : previousTargets) {
        if(target.isNull())
            continue;
        mvwaddch(win, target.y+1, target.x, ' ');
    }
    int i = 0;
    for (const auto& target : targets) {
        i++;
        if(target.isNull())
            continue;
        mvwprintw(win, target.y+1, target.x, "%d", i);
        
    }
    wattroff(win, COLOR_PAIR(2));
    previousTargets = targets;
}


void Visualizer::drawObstacles(WINDOW* win,const std::vector<Point>& obstacles) {
    wattron(win, COLOR_PAIR(3)); // Use orange color

    for (const auto& obstacle : previousObstacles) {
        if(obstacle.isNull())
            continue;
        mvwaddch(win, obstacle.y+1, obstacle.x, ' ');
    }
    for (const auto& obstacle : obstacles) {
        if(obstacle.isNull())
            continue;
        mvwaddch(win, obstacle.y+1, obstacle.x, 'O');
    }
    wattroff(win, COLOR_PAIR(3));
    previousObstacles = obstacles;
}



void Visualizer::draw(WINDOW* win,const Point& dronePosition,const std::vector<Point>& targets,const std::vector<Point>& obstacles)
{
    drawBorders(win);
    drawTargets(win,targets);
    drawObstacles(win,obstacles);
    drawDrone(win,dronePosition);
}
// void Visualizer::updateBorders(const WindowBorders& rect)
// {
//     borders = rect;
// }
void Visualizer::refresh(WINDOW* win)
{
    wrefresh(win);
}


