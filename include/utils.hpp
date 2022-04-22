#pragma once

#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/value.h>
#include <time.h>

#include "math.hpp"

using namespace std;
using namespace Json;

struct Logger;
struct RendererFlags;
struct InterpreterSettings;
struct ConsoleSettings;
struct WindowSettings;

/**
 * @brief Structure that contains strings and functions for logging
*/
struct Logger {
    /**
     * @brief Function to get a string time to log
     * @returns The string time, type string
    */
    string getStringTime();
    /**
     * @brief Function to set the strings for logging
     * @param log If the game will log to file, type bool
     * @param color If the terminal will have color, type bool
    */
    void setColors(ConsoleSettings c);
    /**
     * @brief Function to get the current second
     * @return The second, type float
    */
    float getSecond();
    string reset, success, error, warning, info;
};

/**
 * @brief Structure that contains the renderer flags
 * @param vsync If the renderer will be synced with the monitor refresh rate, type bool
 * @param gpuAcc If the renderer will use the GPU, type bool
 * @param software If the renderer will be a software fallback, type bool
 * @param tagTex If the renderer will support rendering to texture, type bool
*/
struct RendererFlags {
    bool vsync, gpuAcc, software, targTex;
};
/**
 * @brief Structure that contains the window settings
 * @param height The window height, type Uint32
 * @param width The window width, type Uint32
 * @param maxFps The FPS limit, type Uint32
 * @param scale The scale, type float
 * @param guiScale The GUI scale, type float
 * @param fpsCounter If the FPS counter will be displayed, type bool
 * @param flags The renderer flags, type RendererFlags
*/
struct WindowSettings {
    Uint32 height, width, maxFps;
    float scale, guiScale;
    bool fpsCounter;
    RendererFlags flags;
};
/**
 * @brief Structure to contain binary interpreter settings
 * @param file The file name containing the binary, type string
 * @param ramSize The fixed size of the virtual memory avaiable to the virtual system
 * @param start The address of first program code line
*/
struct InterpreterSettings {
    string file;
    Uint16 ramSize, start;
    bool binary;
};
/**
 * @brief Structure to contain binary interpreter settings
 * @param log If it will log to file or terminal, type bool
 * @param color If the terminal will have color, type bool
*/
struct ConsoleSettings {
    bool log, color;
};
/**
 * @brief Structure that contains the settings
 * @param win The window settings, type WindowSettings
 * @param interpreter The interpreter settings, type InterpreterSettings
 * @param console The console settings, type ConsoleSettings
*/
struct Settings {
    WindowSettings win;
    InterpreterSettings interpreter;
    ConsoleSettings console;
};
/**
 * @brief Structure that contains the rect array for the letters
 * @param letters[128] Array with all the rects for each letter
*/
struct Font {
    SDL_Rect letters[128];
};

/**
 * @brief Structure to contain the rect array for the cursor
 * @param pointers[4] The array with all the cursor state pointers
*/
struct Cursor {
    SDL_Rect pointers[4];
};

ostream& operator << (ostream& os, const Settings& settings);

namespace JsonManager {
        /**
         * @brief Function to read the game settings
         * @return Returns the game settings, type Settings
        */
        Settings getSettings();
        /**
         * @brief Function to get the flags from the settings
         * @param settings The settings, type Settings
         * @return Returns the flags for the Renderer, type Uint32
        */
        Uint32 getFlags(Settings settings);
        /**
         * @brief Function to get the rects for the font textures
         * @return Returns the font rects, type Font
        */
        Font getFont();
        /**
         * @brief Function to get the rects for the cursor textures
         * @return Returns the cursor rects, type Cursor
        */
        Cursor getCursor();
};