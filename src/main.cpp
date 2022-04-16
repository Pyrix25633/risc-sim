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

void randFunc(int a) {
    cout << "Here is a button action! " << a << endl;
}

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
    Uint8 cursorState = 0; //0 -> normal, 1 -> hover, 2 -> normal clicked, 3 -> hover clicked
    Cursor cursor = JsonManager::getCursor();
    vector<HitBox2d> hitboxes = {HitBox2d(0, 0, 10, 10), HitBox2d(0, 100, 16, 16)}; //Vector of all hitboxes

    //Interpreter
    SystemBus SB;
    CentralMemory CM(&SB, settings.interpreter.ramSize);
    InputOutputDevices IOD(&SB);
    CentralProcessingUnit CPU(&SB, &CM, &IOD, settings.interpreter.start);
    CM.loadProgram(settings.interpreter.file, settings.interpreter.binary, &logger);
    CPU.fetchInstruction();

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
    SDL_Texture* grassBlockTexture = Window.loadTexture("res/img/grass_block.png");
    SDL_Texture* fontTexture = Window.loadTexture("res/img/font.png");
    vector<Entity> platforms = {Entity(Vector2f(0, 100), grassBlockTexture)/*,
                                Entity(Vector2f(16, 100), grassBlockTexture),
                                Entity(Vector2f(32, 100), grassBlockTexture)*/};
    Entity cursorEntity(Vector2f(0, 0), cursorTexture);
    TextEntity fpsCounterEntity(Vector2f(1, 1), fontTexture, &font);
    Button button1(Vector2f(100, 100), HitBox2d(100, 100, 8, 8), grassBlockTexture);
    fpsCounter = fpsText + fpsString;
    fpsCounterEntity = fpsCounter;
    //CPU
    TextEntity cpuTitle(Vector2f(16, 16), fontTexture, &font);
    TextEntity pcTitle(Vector2f(24, 24), fontTexture, &font);
    TextEntity irTitle(Vector2f(24, 32), fontTexture, &font);
    TextEntity srTitle(Vector2f(24, 40), fontTexture, &font);
    TextEntity arTitle(Vector2f(24, 48), fontTexture, &font);
    TextEntity drTitle(Vector2f(24, 56), fontTexture, &font);
    TextEntity spTitle(Vector2f(24, 64), fontTexture, &font);
    TextEntity pcValue(Vector2f(32, 24), fontTexture, &font);
    TextEntity irValue(Vector2f(32, 32), fontTexture, &font);
    TextEntity srValue(Vector2f(32, 40), fontTexture, &font);
    TextEntity arValue(Vector2f(32, 48), fontTexture, &font);
    TextEntity drValue(Vector2f(32, 56), fontTexture, &font);
    TextEntity spValue(Vector2f(32, 64), fontTexture, &font);
    cpuTitle = "CPU";
    pcTitle = "PC";
    irTitle = "IR";
    srTitle = "SR";
    arTitle = "AR";
    drTitle = "DR";
    spTitle = "SP";
    pcValue = "0x0000";
    irValue = "0x0000";
    srValue = "0000";
    arValue = "0x0000";
    drValue = "0x0000";
    spValue = "0x0000";

    //Running
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
                else if(event.type == SDL_MOUSEBUTTONDOWN) {
                    if(cursorState == 0) cursorState = 2;
                    else if(cursorState == 1) cursorState = 3;
                }
                else if(event.type == SDL_MOUSEBUTTONUP) {
                    if(cursorState == 2) cursorState = 0;
                    else if(cursorState == 3) cursorState = 1;
                }
            }
            //Actual processing
            SDL_GetMouseState(&cursorPosition.x, &cursorPosition.y);
            guiCursorPosition.x = (cursorPosition.x / settings.win.scale / 4);
            guiCursorPosition.y = (cursorPosition.y / settings.win.scale / 4);
            if(guiCursorPosition == *button1.getHitBox()) {
                cursorState = 1;
                button1.action(randFunc);
            }
            else if(cursorState == 1) cursorState = 0;
            cursorEntity.setXY(cursorPosition.x, cursorPosition.y);
            cursorEntity.setCurrentFrame(cursor.pointers[cursorState]);
            Window.clear();
            for(Entity& p : platforms) {
                Window.render(p);
            }
            if(settings.win.fpsCounter) {
                Window.renderText(fpsCounterEntity);
            }

            //CPU Values
            pcValue = "0x" + math::Uint16Tohexstr(CPU.getPC());
            irValue = "0x" + math::Uint16Tohexstr(CPU.getIR());
            srValue = math::StatusRegisterToHexstr(CPU.getSR());
            arValue = "0x" + math::Uint16Tohexstr(CPU.getAR());
            drValue = "0x" + math::Uint16Tohexstr(CPU.getDR());
            spValue = "0x" + math::Uint16Tohexstr(CPU.getSP());
            //CPU Render
            Window.renderText(cpuTitle);
            Window.renderText(pcTitle);
            Window.renderText(irTitle);
            Window.renderText(srTitle);
            Window.renderText(arTitle);
            Window.renderText(drTitle);
            Window.renderText(spTitle);
            Window.renderText(pcValue);
            Window.renderText(irValue);
            Window.renderText(srValue);
            Window.renderText(arValue);
            Window.renderText(drValue);
            Window.renderText(spValue);

            Window.renderButton(button1);
            Window.renderCursor(cursorEntity);
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