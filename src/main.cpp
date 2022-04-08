#include <iostream>
#include <vector>
#include <cstdio>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_audio.h>

#include "renderwindow.hpp"
#include "entity.hpp"
#include "utils.hpp"

using namespace std;

int main(int argc, char* args[]) {
    //Variables
    Logger logger;
    Settings settings = JsonManager::getSettings(); //Variable to store the game settings from the json
    Font font = JsonManager::getFont(); //Variable to store all the letters rects
    bool running = true; //Variable to know if the game has to continue running or not
    SDL_Event event; //Variable to store game Window events
    Uint32 flags = JsonManager::getFlags(settings);
    Uint16 msStep = 1000 / settings.win.maxFps, fps = 1; //Variables to regulate the framerate
    Uint64 previousSecond, currentSecond;
    long int msNow, msNext = SDL_GetTicks();
    string fpsString = "000", fpsCounter, fpsText = "FPS:";
    Vector2d cursorPosition, guiCursorPosition;
    Uint8 cursorState; //0 -> normal, 1 -> hover
    vector<HitBox2d> hitboxes = {HitBox2d(0, 0, 10, 10), HitBox2d(0, 100, 16, 16)}; //Vector of all hitboxes

    //Interpreter
    SystemBus SB;
    CentralMemory CM(&SB, settings.interpreter.ramSize);
    InputOutputDevices IOD(&SB);
    CentralProcessingUnit CPU(&SB, &CM, &IOD, settings.interpreter.start);
    CM.loadProgram(settings.interpreter.file, settings.interpreter.binary, &logger);
    CPU.fetchInstruction();
    cout << *(CPU.getIR()) << endl;

    //Capturing cout in log file
    if(settings.console.log) freopen("log.txt", "w", stdout);
    logger.setColors(settings.console);

    //SDL and IMG initialization
    cout << logger.getStringTime();
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) > 0) {
        cout << logger.error << "SDL Init FAILED! SDL_ERROR: " << SDL_GetError() << logger.reset << endl;
    }
    else {
        cout << logger.success << "SDL Init SUCCEEDED" << logger.reset << endl;
    }
    cout << logger.getStringTime();
    if(!IMG_Init(IMG_INIT_PNG)) {
        cout << logger.error << "IMG Init FAILED! IMG_ERROR: " << SDL_GetError() << logger.reset << endl;
    }
    else {
        cout << logger.success << "IMG Init SUCCEEDED" << logger.reset << endl;
    }
    //Printing the settings
    cout << logger.getStringTime() << logger.info << "Settings:" << endl << settings << logger.reset << endl;
    //Render the window
    RenderWindow Window("GAME v1.0", settings.win.width, settings.win.height, flags, &logger, &settings);
    SDL_ShowCursor(0);

    //Loading the textures
    SDL_Texture* cursorTexture = Window.loadTexture("res/img/cursor.png");
    SDL_Texture* cursorHoverTexture = Window.loadTexture("res/img/cursor_hover.png");
    SDL_Texture* grassBlockTexture = Window.loadTexture("res/img/grass_block.png");
    SDL_Texture* fontTexture = Window.loadTexture("res/img/font.png");
    vector<Entity> platforms = {Entity(Vector2f(0, 100), grassBlockTexture)/*,
                                Entity(Vector2f(16, 100), grassBlockTexture),
                                Entity(Vector2f(32, 100), grassBlockTexture)*/};
    vector<Entity> cursorEntity = {Entity(Vector2f(0, 0), cursorTexture),
                                   Entity(Vector2f(0, 0), cursorHoverTexture)};
    TextEntity fpsCounterEntity(Vector2f(1, 1), fontTexture, &font);
    fpsCounter = fpsText + fpsString;
    fpsCounterEntity = fpsCounter;
    TextEntity someText(Vector2f(100, 200), fontTexture, &font);
    someText = "Contact: biral.mattia@gmail.com 32$ 50% #20";
    TextEntity textEntity(Vector2f(10.0, 15.0), fontTexture, &font);
    textEntity = "Test";
    textEntity += " some text";
    textEntity += platforms[0];

    //Running the game
    previousSecond = SDL_GetTicks() / 1000;
    while(running) {
        msNow = SDL_GetTicks();
        if(!(msNext <= msNow)) {
            //Framerate regulation
            SDL_Delay(msNext - msNow);
        }
        else {
            //FPS counting
            if(settings.win.fpsCounter) {
                currentSecond = msNow / 1000;
                if(currentSecond == previousSecond) {
                    fps++;
                }
                else {
                    previousSecond = msNow / 1000;
                    //cout << logger.getStringTime() << "FPS: " << fps << endl;
                    fpsString = to_string(fps);
                    fpsCounter = fpsText + fpsString;
                    fpsCounterEntity = fpsCounter;
                    fps = 1;
                }
            }
            msNext = msNow + msStep;
            //Controls
            while(SDL_PollEvent(&event)) {
                if(event.type == SDL_QUIT) {
                    running = false;
                    cout << logger.getStringTime() << logger.info << "Game Windwow Closed" << logger.reset << endl;
                }
            }
            //Actual game processing
            SDL_GetMouseState(&cursorPosition.x, &cursorPosition.y);
            guiCursorPosition.x = (cursorPosition.x / settings.win.scale / 4);
            guiCursorPosition.y = (cursorPosition.y / settings.win.scale / 4);
            for(HitBox2d h : hitboxes) {
                if(guiCursorPosition == h) cursorState = 1;
                else cursorState = 0;
            }
            cursorEntity[cursorState].setXY(cursorPosition.x, cursorPosition.y);
            Window.clear();
            for(Entity& p : platforms) {
                Window.render(p);
            }
            if(settings.win.fpsCounter) {
                Window.renderText(fpsCounterEntity);
            }
            Window.renderText(someText);
            Window.renderText(textEntity);
            Window.renderCursor(cursorEntity[cursorState]);
            //Window.playSound();
            Window.display();
        }
    }
    //Quitting Window
    Window.cleanUp();
    SDL_Quit();
    return 0;
}

//For stupid Windows OS
int WinMain(int argc, char* args[]) {
    return main(argc, args);
}