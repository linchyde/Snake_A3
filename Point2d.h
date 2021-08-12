#pragma once
#ifndef __POINT2D__
#define __POINT2D__

namespace textpixels
{
  class Point2d
  {
  public:
    int x = 0;
    int y = 0;

    Point2d();
    Point2d(int x, int y);
    Point2d(const Point2d& other); // copy constructor

    friend Point2d operator + (Point2d const&, Point2d const&);
    friend bool operator == (Point2d const& p1, Point2d const& p2);

    /// Copy assignment operator: point1 = point2; will copy point2 values to point1.
    Point2d& operator =(const Point2d& other);

    /// Copy another Point2d's x and y
    void copy(const Point2d& other);
    /// Test x and y against another Point2d's
    bool isEqualTo(const Point2d& other);

  };
}

#endif