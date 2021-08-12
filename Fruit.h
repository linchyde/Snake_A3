#ifndef __FRUIT__
#define __FRUIT__
#include "Point2d.h"
#include "textpixels_enums.h"
#include "Fruit.h"
using textpixels::Point2d;
using textpixels::Direction;

class Fruit
{
public:

	bool fruitGotAte = false;

  Point2d location { 1,1 };
  Point2d menuLocation{ 1,1 };
  short colour = 0x0;
  int growSnakeby = 1;
  
  int pointsValue = 1;
  //movement and location
};

#endif

