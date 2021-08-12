#pragma once
#include "point2d.h"
namespace textpixels
{
  //////// Constructors

  Point2d::Point2d()
  {}

  /// Constructor with x, y arguments
  Point2d::Point2d(int x, int y)
  {
    this->x = x;
    this->y = y;
  }

  /// Copy constructor takes another Point2D as argument
  Point2d::Point2d(const Point2d& other)
  {
    x = other.x;
    y = other.y;
  }

  /// Operator overloads

  Point2d operator + (Point2d const& p1, Point2d const& p2)
  {
    return Point2d(p1.x + p2.x, p1.y + p2.y);
  }

  bool operator == (Point2d const& p1, Point2d const& p2)
  {
    return (p1.x == p2.x && p1.y == p2.y);
  }

  /// Copy assignment operator: point1 = point2; will copy point2 values to point1.
  Point2d& Point2d::operator =(const Point2d& other)
  {
    // check for self-assignment
    if (&other == this)
      return *this;
    x = other.x;
    y = other.y;
    return *this;
  }

  ///////// Regular methods 

  /// Copy another Point2D's x and y
  void Point2d::copy(const Point2d& other)
  {
    x = other.x;
    y = other.y;
  }


  bool Point2d::isEqualTo(const Point2d& other)
  {
    return(x == other.x && y == other.y);
  }
}