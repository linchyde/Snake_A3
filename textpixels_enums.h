/******************************************************************************
*
* Enums that make textpixels easier to use 👍
* 
* 
* 
* 
* 
* 
* 
*
******************************************************************************/
#pragma once
#ifndef __TEXT_PIXELS_ENUMS__
#define __TEXT_PIXELS_ENUMS__
namespace textpixels {

  enum Direction : int
  {
    RIGHT = 1,
    LEFT = -1,
    UP = -1,
    DOWN = 1,
    NONE = 0
    // add up and down. What is up in y?
  };

  enum Pixel : short
  {
    PIXEL_SOLID = 0x2588,
    PIXEL_THREEQUARTERS = 0x2593,
    PIXEL_HALF = 0x2592,
    PIXEL_QUARTER = 0x2591,
  };

  enum Colour : short
  {
    FG_BLACK = 0x0000,
    FG_DARK_BLUE = 0x0001,
    FG_DARK_GREEN = 0x0002,
    FG_DARK_CYAN = 0x0003,
    FG_DARK_RED = 0x0004,
    FG_DARK_MAGENTA = 0x0005,
    FG_DARK_YELLOW = 0x0006,
    FG_GREY = 0x0007, // Thanks MS :-/
    FG_DARK_GREY = 0x0008,
    FG_BLUE = 0x0009,
    FG_GREEN = 0x000A,
    FG_CYAN = 0x000B,
    FG_RED = 0x000C,
    FG_MAGENTA = 0x000D,
    FG_YELLOW = 0x000E,
    FG_WHITE = 0x000F,
    BG_BLACK = 0x0000,
    BG_DARK_BLUE = 0x0010,
    BG_DARK_GREEN = 0x0020,
    BG_DARK_CYAN = 0x0030,
    BG_DARK_RED = 0x0040,
    BG_DARK_MAGENTA = 0x0050,
    BG_DARK_YELLOW = 0x0060,
    BG_GREY = 0x0070,
    BG_DARK_GREY = 0x0080,
    BG_BLUE = 0x0090,
    BG_GREEN = 0x00A0,
    BG_CYAN = 0x00B0,
    BG_RED = 0x00C0,
    BG_MAGENTA = 0x00D0,
    BG_YELLOW = 0x00E0,
    BG_WHITE = 0x00F0,
    // Layering one colour, fg and bg, gets ride of any lines between "pixels"
    BLACK = FG_BLACK | BG_BLACK,
    BLUE = FG_BLUE | BG_BLUE,
    DARK_BLUE = FG_DARK_BLUE | BG_DARK_BLUE,
    GREEN = FG_GREEN | BG_GREEN,
    DARK_GREEN = FG_DARK_GREEN | BG_DARK_GREEN,
    CYAN = FG_CYAN | BG_CYAN,
    DARK_CYAN = FG_DARK_CYAN | BG_DARK_CYAN,
    RED = FG_RED | BG_RED,
    DARK_RED = FG_DARK_RED | BG_DARK_RED,
    MAGENTA = FG_MAGENTA | BG_MAGENTA,
    DARK_MAGENTA = FG_DARK_MAGENTA | BG_DARK_MAGENTA,
    YELLOW = FG_YELLOW | BG_YELLOW,
    DARK_YELLOW = FG_DARK_YELLOW | BG_DARK_YELLOW,
    GREY = FG_GREY | BG_GREY,
    DARK_GREY = FG_DARK_GREY | BG_DARK_GREY,
    WHITE = FG_WHITE | BG_WHITE

    // Repeat for other colours to avoid lines between PIXEL_SOLID characters
  };

  
}
#endif