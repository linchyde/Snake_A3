#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING // Needed with c++17
/******************************************************************************
*
* main(): 
*   1. Requests textpixels window.
*   2. Provides a loop for our program. It calls for screens to
*      be displayed, listens to their returns to decide what's next.
*   3. Doesn't draw to screen so doesn't need textpixels startFrame etc
*
* playSnake(): 
*   1. Main game play loop. Uses textpixels with a high frames per second
*      to run fast, listen carefully for input.
*   2. Every time n frames have passed, move the snake.
*
* -----------------------------------------------------------------------------
* 
*  

#include <iostream>
#include <string>
#include "Snake.h"
#include "textpixels_enums.h"
#include "textpixels.h"
#include "Fruit.h"
#include <stdlib.h>
#include <conio.h>


using namespace std;
using namespace textpixels;

// A collection of screens you would want in game
enum Screen         
{
  MENU = 1,
  PLAY,
  PAUSE,
  GAMEOVER,
  EXIT,
  NONE
};

// The playing area (snake garden) is above the graphic user interface (GUI)
const int LEVEL_WIDTH = 35;
const int LEVEL_HEIGHT = 20;
const int GUI_HEIGHT = 15;
const int WALL_LEFT = 0;
const int WALL_RIGHT = LEVEL_WIDTH - 1;
const int WALL_TOP = LEVEL_HEIGHT;
const int WALL_BOTTOM = LEVEL_HEIGHT - 20;

// Function declarations
void drawLevelAndGui(Snake snake, int score, Fruit fruit);
int displayMenuScreen();
int playSnake();



/* ------------------------------------------------------------------------- MAIN */

int main()
{
  // textpixels creates our 30x30 console window
  textpixels::setupConsole( LEVEL_WIDTH, LEVEL_HEIGHT+GUI_HEIGHT, 22, 22 );  
  // I chose `screen` as a term for part of the program with its own visuals+controls
  int screen = Screen::MENU;    // Start the game at the menu.
  int score = 0;
  
  while (screen != Screen::EXIT)   // Outermost loop of the whole program
  {
    if (screen == Screen::MENU)
    {
      screen = displayMenuScreen();
    }
    else if (screen == Screen::PLAY)
    {
      score = playSnake();
      screen = Screen::MENU;      // Requesting menu as the next screen after game
    }
  }
  screen = displayMenuScreen();  
  // Add some sort of exit screen
  textpixels::exit();   // Tell textpixels to clean up after itself.
} 

/* -------------------------------------------------------- FUNCTION DEFINITIONS */

// Draw the Level's ground and border wall to top of window,
// draw gui with score and any other info at bottom.
void drawLevelAndGui(Snake snake, int score, Fruit apple)
{
  // Draw the background and a wall.
  fillWindow(YELLOW);
  fillRect(1, 1, LEVEL_WIDTH - 2, LEVEL_HEIGHT - 2, BG_BLACK);
  // Draw the gui background, starting at bottom of level
  fillRect(0, LEVEL_HEIGHT, LEVEL_WIDTH, GUI_HEIGHT, BLACK);
  //if snake hits a wall, write a msg at top of the play area.
  if (snake.didHitWall == true)
  {
      drawString(5, 19, "**** YOU HIT A WALL ****", layerColours(BG_YELLOW, FG_BLACK));
      drawString(5, WALL_TOP - 20, "**** YOU HIT A WALL ****", layerColours(BG_YELLOW, FG_BLACK));
  }
  else (snake.didHitWall == false);

  if (apple.fruitGotAte == true)
  {
      drawString(5, 19, "**** YOU ATE AN APPLE ****", layerColours(BG_YELLOW, FG_BLACK));

  }
  else (apple.fruitGotAte == false);
  

  drawString(1, LEVEL_HEIGHT + 1, "MENU OPTIONS ", FG_CYAN);
  drawString(2, LEVEL_HEIGHT + 3, "(Q) ", FG_CYAN);
  drawString(6, LEVEL_HEIGHT + 3, "quit", FG_GREY);
  drawString(1, LEVEL_HEIGHT + 11, "(EAT THE FRUITS TO SCORE POINTS) ", FG_CYAN);
  drawString(3, LEVEL_HEIGHT + 12, "****EACH FRUIT = 1 POINT****", FG_BLUE);
  drawString(1, LEVEL_HEIGHT + 13, "(APPLE)", FG_CYAN);
  drawString(12, LEVEL_HEIGHT + 13, "(BANANA) ", FG_YELLOW);
  drawString(24, LEVEL_HEIGHT + 13, "(GRAPE) ", FG_CYAN);
  //drawing fps purely for debugging, needs to removed
  //drawString(LEVEL_WIDTH - 9, LEVEL_HEIGHT - 1, "fps:", layerColours(FG_DARK_MAGENTA, BG_GREY));
  //drawString(LEVEL_WIDTH - 5, LEVEL_HEIGHT - 1, textpixels::getFpsString(), layerColours(FG_DARK_MAGENTA, BG_GREY));



  //write the score in the top right
//string myScore = to_string(888);
  drawString(LEVEL_WIDTH - 9, LEVEL_HEIGHT, "SCORE:", layerColours(FG_RED, BG_WHITE));
  //to_string used below to convert the score as an integer to display as a string
  drawString(LEVEL_WIDTH - 3, LEVEL_HEIGHT, to_string(score), layerColours(FG_BLACK, BG_WHITE));

  //drawString(1, LEVEL_HEIGHT + 3, "==== DEBUG INFO =========", layerColours(FG_DARK_GREY, BG_BLACK));
  drawString(24, LEVEL_HEIGHT + 2, "CONTROLS");
  drawString(21, LEVEL_HEIGHT + 3, "MOVE : W,A,S,D");
  drawString(22, LEVEL_HEIGHT + 4, "OR ARROW KEYS");
}



// Responsible for core snake game loop: listening for input,
// simulating changes in the game world, drawing the new state to screen.
// Returns score 
int playSnake()
{
  textpixels::setFps(200);      // Set a high frame rate for input sensitivity.
  // Set up whatever needs to persist throughout the loop
  bool playerHasQuit = false;
  bool didHitWall = false;
  bool fruitGotAte = false;
  int score = 55;

  Snake slithers;            
  slithers.colour = YELLOW;
  //FPS/move interval = speed in pixels per second
  slithers.moveInterval = 15;
  slithers.location.x = 10;    // 5 squares across
  slithers.location.y = 5;
  slithers.xDir = Direction::NONE;
  slithers.yDir = Direction::DOWN;
  //fruit for the snake to eat and grow
  //TODO - needs to add to points
  Fruit apple;
  apple.colour = textpixels::GREEN;
  apple.menuLocation = Point2d(9, windowHeight() - 2);
  apple.location = Point2d(15, 12);
  Fruit banana;
  banana.colour = textpixels::YELLOW;
  banana.menuLocation = Point2d(21, windowHeight() - 2);
  banana.location = Point2d(16, 12);
  Fruit grape;
  grape.colour = textpixels::DARK_MAGENTA;
  grape.menuLocation = Point2d(32, windowHeight() - 2);
  grape.location = Point2d(17, 12);


  
  do
  {
    textpixels::startFrame();   // Let textpixels know we're doing stuff in this frame.

    /* ---------------------------------------------------------------------- INPUT */
    if (keyIsDown('Q'))
    {
      playerHasQuit = true;
      break;  // End the DO loop now, go to the line after it ends (return score?)
    }
    // Check if a, d, left/right arrow is pressed, store it 
    Direction xDirectionInput = textpixels::getLeftRightInput();
    Direction yDirectionInput = textpixels::getUpDownInput();
    
    
    /* ---------------------------------------------------------------- PROCESSING */
    if (xDirectionInput != Direction::NONE)
    {
      // Change our snake direction to match the direction that was entered
        slithers.xDir = xDirectionInput;
        slithers.yDir = Direction::NONE;
    }
    else if (yDirectionInput != Direction::NONE)
    {
        slithers.yDir = yDirectionInput;
        slithers.xDir = Direction::NONE;
    }
        
    // If the snake's moveInterval divides evenly into the current frame count, move her.
    if (textpixels::frame() % slithers.moveInterval == 0)  
    {
      slithers.location.x += slithers.xDir; // xDir is -1, 0 or 1 (see Direction enum) 
      slithers.location.y += slithers.yDir;
    }

    //set an alert when the player hits one of the boundaries
    if (slithers.location.x == WALL_LEFT || slithers.location.x == WALL_RIGHT)
    {
        slithers.didHitWall = true;
     }    
    else if (slithers.location.y == WALL_TOP || slithers.location.y == WALL_BOTTOM)
    {
        slithers.didHitWall = true;
        playerHasQuit = true;
    }

    
    if (slithers.location == apple.location)
    {
        apple.fruitGotAte = true;
    }
    /* ---------------------------------------------------------------- OUTPUT */
    

    drawLevelAndGui(slithers, score, apple);
        
    drawPixel(apple.location, apple.colour);
    drawPixel(apple.menuLocation, apple.colour);
    drawPixel(banana.location, banana.colour);
    drawPixel(banana.menuLocation, banana.colour);
    drawPixel(grape.location, grape.colour);
    drawPixel(grape.menuLocation, grape.colour);

    drawPixel(slithers.location, slithers.colour);
    // TODO: Fix line above so slithers has her correct colour.
    textpixels::endFrame();     
  } while (!playerHasQuit); 

  return score; // Return placeholder score for now
}

// Returns the player's choice using a constant from enum Screen.
int displayMenuScreen()
{
  // 60fps is plenty fast for a menu.
  textpixels::setFps(60);
  int choice = Screen::NONE;
  do                            // Keeps looping, waiting for input
  {
    textpixels::startFrame();   // Needed always at start of game loop
    fillWindow(YELLOW);
    fillRect(1, 1, windowWidth() - 2, windowHeight() - 2, BG_BLACK);
    drawString(10, 13, "(P) PLAY SNAKE", layerColours(FG_YELLOW, BG_BLACK));
    drawString(13, 16, "(X) EXIT", layerColours(FG_RED, BG_BLACK));
    

     if (keyIsDown('P'))
    {
      choice = Screen::PLAY;
    }
    else if (keyIsDown('X'))
    {
      choice = Screen::EXIT;
    }
    textpixels::endFrame();     // Needed always at end of game loop.
  } while (choice == Screen::NONE);     // Only stop when playerHasQuit  
  return(choice);
}
