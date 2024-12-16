#pragma once
#include<iostream>
#include<math.h>
struct Point
{
    double x;
    double y;
    bool isNull() const {
        return x == NAN || y == NAN; // Define your own criteria for "null"
    }
    // Overload == operator to compare points
    bool operator==(const Point& other) const {
        
        return x == other.x && y == other.y;
    }
        // Overload + operator for point addition
    Point operator+(const Point& other) const {
        return Point{x + other.x, y + other.y};
    }

    Point operator-(const Point& other) const {
        return Point{x - other.x, y - other.y};
    }

    // Overload * operator for scalar multiplication
    Point operator*(double scalar) const {
        return Point{x * scalar, y * scalar};
    }
    Point operator*(const Point& other) const {
        return Point{x * other.x, y * other.y};
    }

    // Overload / operator for scalar division
    Point operator/(double scalar) const {
        if (scalar == 0) {
            throw std::invalid_argument("Division by zero is not allowed.");
        }
        return Point{x / scalar, y / scalar};
    }
    double dist(const Point& other) const{
        return pow(pow(x-other.x,2)+pow(y-other.y,2),0.5);
    }
    // Overload << operator to print the point
    friend std::ostream& operator<<(std::ostream& os, const Point& point) {
        os << "(" << point.x << ", " << point.y << ")";
        return os;
    }

};

// Overload / operator for scalar / Point
inline Point operator/(double scalar, const Point& point) {
    if (point.x == 0 || point.y == 0) {
        throw std::invalid_argument("Division by zero is not allowed for a Point with zero coordinates.");
    }
    return Point{scalar / point.x, scalar / point.y};
}

// Overload / operator for scalar / Point
inline Point operator*(double scalar, const Point& point) {

    return Point{scalar * point.x, scalar * point.y};
}


// Overload pow function for Point
inline Point pow(const Point& point, double exponent) {
    return Point{ 
        pow(point.x, exponent), 
        pow(point.y, exponent) 
    };
}