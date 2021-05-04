#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
# include <cmath>


class Point{
private:
    double x;
    double y;
public:
    // default constructor
    Point();

    void move(double dx, double dy);

    double distanceFrom(const Point & p);
};

// #include "point.hpp"

Point::Point():x(0), y(0){}

void Point::move(double dx, double dy){
        x = x + dx;
        y = y + dy;
    }

double Point::distanceFrom(const Point & p){
        double ans = std::sqrt((x - p.x)*(x - p.x)+(y - p.y)*(y - p.y));
        return ans;
    }

////////////// circle.hpp /////////////////////
// #include "point.hpp"
class Circle{
private:
    Point center;
    const double radius;
public:
    Circle(Point c, double r);
    void move(double dx, double dy);
    double intersectionArea(const Circle & otherCircle);
};

////////// circle.cpp ////////////////////////////

// # include "circle.hpp"
# define PI 3.14159265358979

Circle::Circle(Point c, double r): center(c), radius(r){}

void Circle::move(double dx, double dy){
    center.move(dx, dy);
}

double Circle::intersectionArea(const Circle & otherCircle){
    const double dis = center.distanceFrom(otherCircle.center);
    // no intersection
    if (dis >= radius + otherCircle.radius){
        return 0.0;
    }
    // included
    if (dis <= std::abs(radius - otherCircle.radius)){
        return pow(fmin(radius, otherCircle.radius), 2) * PI;
    } 
    // intersection
  const double r1 = fmax(radius, otherCircle.radius);
  const double r2 = fmin(radius, otherCircle.radius);
  const double r1_sq = pow(r1, 2);
  const double r2_sq = pow(r2, 2);
  const double x = (r1_sq - r2_sq + pow(dis, 2)) / (2 * dis);
  const double y = sqrt(r1_sq - pow(x, 2));
  const double alpha = atan2(y, x);
  const double beta = atan2(y, dis - x);                                                                         
  const double S = dis * y;
  const double S1 = alpha * r1_sq;
  const double S2 = beta * r2_sq;
  return S1 + S2 - S;

}

