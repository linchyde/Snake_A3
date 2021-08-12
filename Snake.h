#ifndef __SNAKE__
#define __SNAKE__
#include "Point2d.h"
#include "textpixels_enums.h"
using textpixels::Point2d;
using textpixels::Direction;
//todo include the fruit

class Snake
{
public:
	//other properties
  short colour = 0x0;
  bool didHitWall = false;
  //other properties
  
  // Position and speed use data types/enums are defined in textpixels_enums.h
  //movement and location
  Point2d location { 1,1 };
  int xDir = Direction::NONE;
  int yDir = Direction::NONE;
  //move on evry nth frame
  int moveInterval = 10;
  //movement and location
};

#endif

