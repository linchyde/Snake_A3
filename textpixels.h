#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING  // Add this to main if it isn't doing the job.
/** TextPixels.h, a bumpercar remix of olcConsoleGameEngine for new programmers.
 *
 *  @author Danny McGillick
 *  @author Javidx9
 *  Contact: dmac_makes#3457 (Discord)
 *
 *---- Original credits follow ------------------------
 *  OneLoneCoder.com - Command Line Game Engine
 *  "Who needs a frame buffer?" - @Javidx9
 *  The Original & Best :P
 *  One Lone Coder License
 *  Original Copyright (C) 2018 Javidx9
 */
#pragma once
#pragma comment(lib, "winmm.lib")
#include <Windows.h>
#include <chrono>
#include <functional>
#include <sstream>
#include <thread>
#include <wchar.h>
#include "textpixels_enums.h"
#include <codecvt>
#include "Point2d.h"
using namespace std::chrono_literals;

#ifndef __TEXT_PIXELS__
#define __TEXT_PIXELS__
using namespace std;
using namespace std::chrono;

namespace textpixels
{

  class TextPixels
  {
  public:
    int desiredFps = 30;
    microseconds desiredFrameTime = 333us;
    milliseconds maxFrameTime_ms = 33ms;
    microseconds maxFrameTime_us = 333us;
    unsigned int frame = 0;            // count frames.
    int windowWidth;
    int windowHeight;
    int fontWidth = 12;
    int fontHeight = 12;
    time_point<high_resolution_clock> thisFrameStarted = high_resolution_clock::now();
    time_point<high_resolution_clock> lastFrameStarted = high_resolution_clock::now();
    time_point<high_resolution_clock> lastFrameEnded = high_resolution_clock::now();
    //time_point<high_resolution_clock> tp2 = high_resolution_clock::now();

    HANDLE console;
    HANDLE consoleIn;
    HANDLE originalConsole;
    SMALL_RECT rectWindow;
    CHAR_INFO* bufScreen;

    static void exit()
    {
      timeEndPeriod(3);
    }

    int Error(const wchar_t* msg)
    {
      wchar_t buf[256];
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
      SetConsoleActiveScreenBuffer(originalConsole);
      //wprintf(L"ERROR: %s\n\t%s\n", msg, buf); FIX
      return 0;
    }
    static BOOL CloseHandler(DWORD evt)
    {
      // Note this gets called in a seperate OS thread, so it must
      // only exit when the game has finished cleaning up, or else
      // the process will be killed before OnUserDestroy() has finished
      if (evt == CTRL_CLOSE_EVENT)
      {
        //m_bAtomActive = false;

        // Wait for thread to be exited
        //std::unique_lock<std::mutex> ul(m_muxGame);
        //m_cvGameFinished.wait(ul);
        exit();
      }
      return true;
    }

    int setupConsole(int width = 20, int height = 20, int characterWidth = 12, int characterHeight = 12)
    {
      timeBeginPeriod(3);
      console = GetStdHandle(STD_OUTPUT_HANDLE);
      consoleIn = GetStdHandle(STD_INPUT_HANDLE);

      fontWidth = characterWidth;
      fontHeight = characterHeight;
      if (console == INVALID_HANDLE_VALUE)
        return Error(L"Bad Handle");

      windowWidth = width;
      windowHeight = height;

      // Update 13/09/2017 - It seems that the console behaves differently on some systems
      // and I'm unsure why this is. It could be to do with windows default settings, or
      // screen resolutions, or system languages. Unfortunately, MSDN does not offer much
      // by way of useful information, and so the resulting sequence is the reult of experiment
      // that seems to work in multiple cases.
      //
      // The problem seems to be that the SetConsoleXXX functions are somewhat circular and 
      // fail depending on the state of the current console properties, i.e. you can't set
      // the buffer size until you set the screen size, but you can't change the screen size
      // until the buffer size is correct. This coupled with a precise ordering of calls
      // makes this procedure seem a little mystical :-P. Thanks to wowLinh for helping - Jx9

      // Change console visual size to a minimum so ScreenBuffer can shrink
      // below the actual visual size
      rectWindow = { 0, 0, 1, 1 };
      SetConsoleWindowInfo(console, TRUE, &rectWindow);

      // Set the size of the screen buffer
      COORD coord = { (short)windowWidth, (short)windowHeight };
      if (!SetConsoleScreenBufferSize(console, coord))
        Error(L"SetConsoleScreenBufferSize");

      // Assign screen buffer to the console
      if (!SetConsoleActiveScreenBuffer(console))
        return Error(L"SetConsoleActiveScreenBuffer");

      // Set the font size now that the screen buffer has been assigned to the console
      CONSOLE_FONT_INFOEX cfi;
      cfi.cbSize = sizeof(cfi);
      cfi.nFont = 0;
      cfi.dwFontSize.X = fontWidth;
      cfi.dwFontSize.Y = fontHeight;
      cfi.FontFamily = FF_DONTCARE;
      cfi.FontWeight = FW_NORMAL;

      /*	DWORD version = GetVersion();
        DWORD major = (DWORD)(LOBYTE(LOWORD(version)));
        DWORD minor = (DWORD)(HIBYTE(LOWORD(version)));*/

        //if ((major > 6) || ((major == 6) && (minor >= 2) && (minor < 4)))		
        //	wcscpy_s(cfi.FaceName, L"Raster"); // Windows 8 :(
        //else
        //	wcscpy_s(cfi.FaceName, L"Lucida Console"); // Everything else :P

        //wcscpy_s(cfi.FaceName, L"Liberation Mono");
      wcscpy_s(cfi.FaceName, L"Consolas");
      if (!SetCurrentConsoleFontEx(console, false, &cfi))
        return Error(L"SetCurrentConsoleFontEx");

      // Get screen buffer info and check the maximum allowed window size. Return
      // error if exceeded, so user knows their dimensions/fontsize are too large
      CONSOLE_SCREEN_BUFFER_INFO csbi;
      if (!GetConsoleScreenBufferInfo(console, &csbi))
        return Error(L"GetConsoleScreenBufferInfo");
      if (windowHeight > csbi.dwMaximumWindowSize.Y)
        return Error(L"Screen Height / Font Height Too Big");
      if (windowWidth > csbi.dwMaximumWindowSize.X)
        return Error(L"Screen Width / Font Width Too Big");

      // Set Physical Console Window Size
      rectWindow = { 0, 0, (short)windowWidth - 1, (short)windowHeight - 1 };
      if (!SetConsoleWindowInfo(console, TRUE, &rectWindow))
        return Error(L"SetConsoleWindowInfo");

      // Set flags to allow mouse input		
      if (!SetConsoleMode(consoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
        return Error(L"SetConsoleMode");

      // Allocate memory for screen buffer
      bufScreen = new CHAR_INFO[windowWidth * windowHeight];
      memset(bufScreen, 0, sizeof(CHAR_INFO) * windowWidth * windowHeight);

      SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseHandler, TRUE);
      return 1;
    }

    void drawBufferToConsole()
    {
      WriteConsoleOutput(console, bufScreen, { (short)windowWidth, (short)windowHeight }, { 0,0 }, &rectWindow);
    }

    void drawCharacter(int x, int y, short c = 0x2588, short col = 0x000F)
    {
      if (x >= 0 && x < windowWidth && y >= 0 && y < windowHeight)
      {
        bufScreen[y * windowWidth + x].Char.UnicodeChar = c;
        bufScreen[y * windowWidth + x].Attributes = col;
      }
    }

    void clip(int& x, int& y)
    {
      if (x < 0) x = 0;
      if (x >= windowWidth) x = windowWidth;
      if (y < 0) y = 0;
      if (y >= windowHeight) y = windowHeight;
    }

    void fillRect(int x1, int y1, int x2, int y2, short col = 0x000F, short c = 0x2588)
    {
      clip(x1, y1);
      clip(x2, y2);
      for (int x = x1; x < x2; x++)
        for (int y = y1; y < y2; y++)
          drawCharacter(x, y, c, col);
    }

    // Use foreground colour, this will use solid pixel
    void fillWindow(short colour)
    {
      fillRect(0, 0, windowWidth+1, windowHeight + 1, colour);
    }

    /**
     *  \brief Set the desired frames to be drawn per second.
     *  Accurate within 1-3 frames if performance allows.
     *  \param desiredFps Defaults to 30. Try 60 if you have faster animation, even 100.
     **/
    void setFps(int fps = 30)
    {
      TextPixels::desiredFps = fps;
      // Take the floor of this number, since we're stuck with millisecond resolution.
      // Better to set a shorter maxFrameTime than a long one and run slow because we 
      // are oversleeping. Casting to int will floor it by default. Would be great if
      // floor could return, you know, ints, whole numbers.
      maxFrameTime_us = microseconds(1'000'000 / fps);
      //maxFrameTime_ms = duration_cast<milliseconds>(maxFrameTime_us);

      //float maxFrameTime_ms_float = maxFrameTime_us.count() / 1000.0f;
      //float blah = 0.0f;
      // next time use float for maxFrameTime and ditch chrono in favour 
      // of the MS QueryPerformanceCounter 
      // https://docs.microsoft.com/en-us/windows/win32/sysinfo/acquiring-high-resolution-time-stamps
    }

    void startFrame()
    {
      thisFrameStarted = std::chrono::high_resolution_clock::now();
      frame++;
    }

    chrono::microseconds getLastFrameDuration_us()
    {
      return(duration_cast<microseconds>(lastFrameEnded - lastFrameStarted));
    }
    
    /*float getLastFrameDuration_sec()
    {
      std::chrono::duration<float> elapsedTime = (lastFrameEnded - lastFrameStarted);
      return(elapsedTime.count());
    }


    int getLastFrameDuration_ms()
    {
      int elapsedTime = static_cast<int>(std::chrono::round<milliseconds>(lastFrameEnded - lastFrameStarted).count());
      return(elapsedTime);
    }
    */
    void drawString(int x, int y, std::wstring c, short col = 0x000F)
    {
      for (size_t i = 0; i < c.size(); i++)
      {
        bufScreen[y * windowWidth + x + i].Char.UnicodeChar = c[i];
        bufScreen[y * windowWidth + x + i].Attributes = col;
      }
    }

    void endFrame()
    {
      drawBufferToConsole();

      // Round up time math: better to sleep too little than too long.
      auto thisFrameLength_us = duration_cast<microseconds>(high_resolution_clock::now() - thisFrameStarted);
      // Subtract an extra 1 ms for the round-down of half or sub millisecond frame processing times.
      this_thread::sleep_for(maxFrameTime_us - thisFrameLength_us - 1ms); 
      
      // In our (frame's) time of dying, write our present and past to history.
      // We want the processing time plus the sleep time or our fps count will be off.
      lastFrameStarted = thisFrameStarted;
      lastFrameEnded = high_resolution_clock::now();
    }

    void drawRow(int y, int colour = (int)Colour::FG_MAGENTA)
    {
      for (int x = 0; x < windowWidth; x++)
      {
        drawCharacter(x, y, 0x2588, colour);
      }
    }

    void drawPixel(int x, int y, short colour = 0x000F)
    {
      drawCharacter(x, y, 0x2588, colour);
    }

    short layerColours(short foreground, short background)
    {
      return (foreground | background);
    }

    /*
      int getFps()
    {
      return((int)(round(1.0f / getLastFrameDuration_sec())));
      //return((int)(ceil(1000.0f / float(getLastFrameDuration_ms()))));
    }
    */

    chrono::milliseconds getLastFrameDuration_ms()
    {
      return(duration_cast<milliseconds>(lastFrameEnded - lastFrameStarted));
    }

    float getFps()
    {
      return(1000000.0f / getLastFrameDuration_us().count());
    }

    std::string getLastFrameDuration_ms_string()
    {
      return(to_string( getLastFrameDuration_ms().count() ));
      //return(to_string( duration_cast<milliseconds>(getLastFrameDuration_us()).count() ));
    }

    std::string getLastFrameDuration_us_string()
    {

      return(to_string(getLastFrameDuration_us().count()));
      //return(to_string( duration_cast<milliseconds>(getLastFrameDuration_us()).count() ));
    }

    std::string getFpsString()
    {
      std::cout.precision(2);
      //return(to_string(std::roundf(getFps()) ));
      return(to_string(static_cast<int>( std::roundf(getFps()) )));
    }

    
  };  // End class TextPixels

////////////// Functions providing no-knowledge access to object //////////////////////

  wstring_convert<codecvt_utf16<wchar_t, 0x10ffff, little_endian>,
    wchar_t> conv;



  std::wstring to_wstr(std::string regularString)
  {
    std::wstringstream cls;
    cls << regularString.c_str();

    return(cls.str());
  }

  std::wstring to_wstr(std::u16string uString)
  {
    wstring converted = conv.from_bytes(
                     reinterpret_cast<const char*> (&uString[0]),
                     reinterpret_cast<const char*> (&uString[0] + uString.size()));
    return converted;
  }

  u16string to_ustring(std::string regularString)
  {
    u16string converted = u16string( regularString.begin(), regularString.end() );

    return(converted);
  }
  
  u16string to_ustring(int regularInt)
  {
    string intString = std::to_string(regularInt);
    u16string converted = u16string( intString.begin(), intString.end() );

    return(converted);
  }
  TextPixels txtPxls;

  // Draw/print a string of characters to the screen at location x,y (in text pixels).
// Use layerColour to supply both foreground and background colours.
  void drawString(int x, int y, std::string regularString, short colour = 0x000F)
  {
    wstring wideString = to_wstr(regularString);
    //wstring::
    txtPxls.drawString(x, y, wideString, colour);
  }

  // Draw/print a wstring of unicode characters to the screen at location x,y (in text pixels).
  // Use layerColour to supply both foreground and background colours.
  // To supply a wstring literal you can use L"whatever", rather than the regular string "example".
  void drawWString(int x, int y, std::wstring wideString, short colour = 0x000F)
  {
    txtPxls.drawString(x, y, wideString, colour);
  }


  // Draw/print a u16string of unicode characters to the screen at location x,y (in text pixels).
  // Use layerColour to supply both foreground and background colours.
  // To supply a wstring literal you can use u"whatever", rather than the regular string "example".
  void drawUString(int x, int y, std::u16string uString, short colour = 0x000F)
  {
    txtPxls.drawString(x, y, to_wstr(uString), colour);
  }
  
  // Checks if the key is currently held down (including first press). You can pass in a char 
  // variable/literal like 'F' or, for special keys, a microsoft key code from Winuser.h
  // https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
  bool keyIsDown(int keyCode)
  {
    return (GetAsyncKeyState(keyCode) < 0);
  }
  
  // Checks for A, D, <- or -> arrow key press
    // Returns Direction::LEFT or RIGHT or NONE.
  textpixels::Direction getLeftRightInput()
  {
    textpixels::Direction dir = textpixels::Direction::NONE;
    if (keyIsDown('A') || keyIsDown(VK_LEFT))
    {
      dir = textpixels::Direction::LEFT;
    }
    else if (keyIsDown('D') || keyIsDown(VK_RIGHT))
    {
      dir = textpixels::Direction::RIGHT;
    }

    return dir;
  }

  // Checks for w, s, up or down arrow key press
  // Returns Direction::UP or DOWN or NONE.
  textpixels::Direction getUpDownInput()
  {
    textpixels::Direction dir = textpixels::Direction::NONE;
    if (keyIsDown('W') || keyIsDown(VK_UP))
    {
      dir = textpixels::Direction::UP;
    }
    else if (keyIsDown('S') || keyIsDown(VK_DOWN))
    {
      dir = textpixels::Direction::DOWN;
    }

    return dir;
  }

 // Width of window in textpixels
  int windowWidth()
  {
    return txtPxls.windowWidth;
  }

  // Height of window in textpixels
  int windowHeight()
  {
    return txtPxls.windowHeight;
  }
  
  /// Current frame as counted from start of game. 
  int frame()
  {
    return (txtPxls.frame);
  }


  int setupConsole(int width = 30, int height = 30, int pixelWidth = 14, int pixelHeight = 14)
  {
    int result = txtPxls.setupConsole(width, height, pixelWidth, pixelHeight);
    //windowWidth = txtPxls.windowWidth;
    //windowHeight = txtPxls.windowHeight;
    return(result);
  }

  // Draw a text character that looks like a full pixel to location x, y on the screen
  void drawPixel(int x, int y, short colour = FG_WHITE)
  {
    txtPxls.drawPixel(x, y, short(colour));
  }

  // Draw a text character that looks like a full pixel to location x, y on the screen
  void drawPixel(Point2d location, short colour = FG_WHITE)
  {
    txtPxls.drawPixel(location.x, location.y, short(colour));
  }

  // Draw a unicode (wide) character to x,y on the screen. 
  // To supply a wide char literal, use L`a` (where a is a character). You can store
  // a wide char in a short int or a wchar_t variable.
  void drawWCharacter(int x, int y, short character, short colour = FG_WHITE)
  {
    txtPxls.drawCharacter(x, y, character, colour);
  }

  // Draw a unicode (16 bit) character to x,y on the screen. 
  // To supply a unicode 16 char literal, use u'a' (where a is a character). You can store
  // a unicode 16 char in a char16_t variable. Try cutting and pasting some box drawing
  // unicode characters from https://en.wikipedia.org/wiki/Box-drawing_character#Unicode 
  void drawUCharacter(int x, int y, char16_t character, short colour = FG_WHITE)
  {
    txtPxls.drawCharacter(x, y, character, colour);
  }

  // Draw a regular char to x,y on the screen. 
  // Supply a char literal such as 'a' or a char variable.
  void drawCharacter(int x, int y, char character, short colour = FG_WHITE)
  {
    txtPxls.drawCharacter(x, y, character, colour);
  }

  // Draw a filled rectangle given coordinates for its top left corner and its width, height in big pixels.
  // @param colour is a short int, but you can use a simple colour constant from the Colours enum: WHITE, RED, DARK_RED etc
  // @param character is unicode, and defaults to a solid pixel. You can substite a wchar or another constant from the Pixel enum, 
  //  but remember to use layerColor(fgcolor, bgcolor) or you'll get a filled pixel.
  void fillRect(int xTopLeft, int yTopLeft, int width=6, int height=4, short colour = WHITE, short character = Pixel::PIXEL_SOLID )
  {
    txtPxls.fillRect(xTopLeft, yTopLeft, xTopLeft+width, yTopLeft+height, colour, character );
  }

  // Draw a filled rectangle given coordinates for its top left and bottom right corners. Remember we're counting up from 0,
  // so you have to subtract 1 if you're using screenwidth/screenheight :D
  // @param colour is a short int, but you can use a simple colour constant from the Colours enum: WHITE, RED, DARK_RED etc
  // @param character is unicode, and defaults to a solid pixel. You can substite a wchar or another constant from the Pixel enum, 
  //  but remember to use layerColor(fgcolor, bgcolor) or you'll get a filled pixel.
  void fillRectByCoords(int xTopLeft, int yTopLeft, int xBotRight, int yBotRight, short colour = FG_WHITE, short character = Pixel::PIXEL_SOLID)
  {
    txtPxls.fillRect(xTopLeft, yTopLeft, xBotRight+1, yBotRight+1, colour, character);
  }

  // Fill the window with colour. Use an FG_ colour as this uses solid pixel.
  void fillWindow(short colour)
  {
    txtPxls.fillWindow(colour);
  }

  // Sets foreground and background colours, useful for drawing
  // strings or chars. Requires an FG colour and a BG colour
  // from the Colour enum in textpixels_enums.h, as follows:
  // layerColours(FG_DARK_RED, BG_GREY) or
  // layerColours(Colours::FG_DARK_RED, Colours::BG_GREY)
  short layerColours(short foreground, short background)
  {
    return (txtPxls.layerColours(foreground, background));
  }

  // Anything drawn with drawPixel/drawString/etc this frame will now
  // be drawn to the screen in a single pass (from the screen buffer)
  void updateScreen()
  {
    txtPxls.drawBufferToConsole();
  }

  // Required at start of game loop when using textpixels.h
  void startFrame()
  {
    txtPxls.startFrame();
    return;
  }

  // Required at end of game loop when using textpixels.h
  void endFrame()
  {
    txtPxls.endFrame();
    return;
  }
  
  /*
  // How long was the time between the start of last frame and
  // the start of this one? In seconds.
  // Returns a float from 0 to 1. eg: 0.5 is half a second, 0.033 is 33 milliseconds.
  float getLastFrameDuration_sec()
  {
    return(txtPxls.getLastFrameDuration_sec());
  }

  // How long was the time between the start of last frame and
  // the start of this one? In milliseconds.
  // Returns an int: 500 is half a second, 33 is.. 33 milliseconds.
  int getLastFrameDuration_ms()
  {
    return (txtPxls.getLastFrameDuration_ms());
  }
  */
//    std::function<void(int, int, std::wstring, short)> printWString = [=](int x, int y, std::wstring aWString, short colour = 0x000F) {
//    return txtPxls.drawString(x, y, aWString, colour);
//  };



  //  Set the desired frames to be drawn per second.
  //  Accurate within 1-3 frames if performance allows.
  //  @param desiredFps Defaults to 30. Try 60 if you have faster animation, even 100.
  void setFps(int desiredFps = 30)
  {
    txtPxls.setFps(desiredFps);
  }

  // The number of frames being  drawn per second. Highly changeable since it only 
  // checks the last frame.
  float getFps()
  {
    return(txtPxls.getFps());
  }

  // The number of frames being  drawn per second as a string. Highly changeable 
  // since it only checks the last frame.
  std::string getFpsString()
  {
    return(txtPxls.getFpsString());
  }

  std::string getLastFrameDuration_ms_string()
  {
    return(txtPxls.getLastFrameDuration_ms_string());
  }

  std::string getLastFrameDuration_us_string()
  {
    return(txtPxls.getLastFrameDuration_us_string());
  }

  void exit()
  {
    TextPixels::exit();
  }
}

#endif