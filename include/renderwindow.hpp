#pragma once
#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entity.hpp"
#include "utils.hpp"

using namespace std;

/** 
 * @brief Class to render a window, contains the window and the renderer
*/
class RenderWindow {
    public:
        /**
         * @brief Constructor
         * @param title The window title, type char*
         * @param width The window width, type int
         * @param height The window height, type int
         * @param flags The renderer flags, type Uint32
         * @param plogger The logger, type Logger*
         * @param psettings The game settings, type GameSettings*
         * @param icon The icon image path
        */
        RenderWindow(const char* title, int width, int height, Uint32 flags, Logger* plogger, Settings* psettings, const char* icon);
        /**
         * @brief Function to load a texture
         * @param filePath The file path, type char*
         * @return The texture, type SDL_Texture*
        */
        SDL_Texture* loadTexture(const char* filePath);
        /**
         * @brief Function to clear the window
        */
        void clear();
        /**
         * @brief Function to render a gui
         * @param entity The gui entity to render, type Entity
        */
        void renderGui(Entity& entity);
        /**
         * @brief Function to render a text entity
         * @param textEntity The text entity to render, type TextEntity
        */
        void renderText(TextEntity& textEntity);
        /**
         * @brief Function to render a button
         * @param button The button to render, type Button
        */
        void renderButton(Button& button);
        /**
         * @brief Function to render the cursor
         * @param entity The cursor entity to render, type Entity
        */
        void renderCursor(Entity& entity);
        /**
         * @brief Function to display the rendered window
        */
        void display();
        /**
         * @brief Function to destroy the window and clean up everything
        */
        void cleanUp();
        /**
         * @brief Function to calculate the scale
        */
        void calculateScale();
        /**
         * @brief Function to get the scale
         * @return The scale, type Uint8
        */
        Uint8 getScale();
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        Logger* logger;
        Settings* settings;
        Uint8 scale;
};