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
    Uint8 inHitboxes = 0;
    bool clicked = false;

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
    SDL_Texture* fontTexture = Window.loadTexture("res/img/font.png");
    SDL_Texture* progressBarTexture = Window.loadTexture("res/img/progress_bar.png");
    SDL_Texture* progressBarNowTexture = Window.loadTexture("res/img/progress_bar_now.png");
    SDL_Texture* progressBarNextTexture = Window.loadTexture("res/img/progress_bar_next.png");
    SDL_Texture* progressBarAllTexture = Window.loadTexture("res/img/progress_bar_all.png");
    SDL_Texture* fastTexture = Window.loadTexture("res/img/fast_button.png");
    SDL_Texture* playTexture = Window.loadTexture("res/img/play_button.png");
    SDL_Texture* nextTexture = Window.loadTexture("res/img/next_button.png");
    SDL_Texture* pauseTexture = Window.loadTexture("res/img/pause_button.png");
    SDL_Texture* stopTexture = Window.loadTexture("res/img/stop_button.png");
    SDL_Texture* fastPressedTexture = Window.loadTexture("res/img/fast_button_pressed.png");
    SDL_Texture* playPressedTexture = Window.loadTexture("res/img/play_button_pressed.png");
    SDL_Texture* nextPressedTexture = Window.loadTexture("res/img/next_button_pressed.png");
    SDL_Texture* pausePressedTexture = Window.loadTexture("res/img/pause_button_pressed.png");
    SDL_Texture* stopPressedTexture = Window.loadTexture("res/img/stop_button_pressed.png");
    Entity cursorEntity(Vector2f(0, 0), cursorTexture);
    TextEntity fpsCounterEntity(Vector2f(1, 1), fontTexture, &font);
    //Progress bar
    TextEntity progressBarIfTitle(Vector2f(178, 1), fontTexture, &font);
    TextEntity progressBarIdTitle(Vector2f(186, 1), fontTexture, &font);
    TextEntity progressBarOfTitle(Vector2f(194, 1), fontTexture, &font);
    TextEntity progressBarIeTitle(Vector2f(202, 1), fontTexture, &font);
    Entity progressBarEntity(Vector2f(178, 4), progressBarTexture, 16, 64);
    Entity progressBarNowEntity(Vector2f(178, 4), progressBarNowTexture);
    Entity progressBarNextEntity(Vector2f(186, 4), progressBarNextTexture);
    //Buttons
    Button fastButton(Vector2f(211, 1), HitBox2d(211, 1, 7, 7), fastTexture, fastPressedTexture);
    Button playButton(Vector2f(220, 1), HitBox2d(220, 1, 7, 7), playTexture, playPressedTexture);
    Button nextButton(Vector2f(229, 1), HitBox2d(229, 1, 7, 7), nextTexture, nextPressedTexture);
    Button pauseButton(Vector2f(238, 1), HitBox2d(238, 1, 7, 7), pauseTexture, pausePressedTexture);
    Button stopButton(Vector2f(247, 1), HitBox2d(247, 1, 7, 7), stopTexture, stopPressedTexture);
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
    progressBarIfTitle = "IF";
    progressBarIdTitle = "ID";
    progressBarOfTitle = "OF";
    progressBarIeTitle = "IE";
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
            clicked = false;
            //Controls
            while(SDL_PollEvent(&event)) {
                if(event.type == SDL_QUIT) {
                    running = false;
                    cout << logger.getStringTime() << logger.info << "Game Windwow Closed" << logger.reset << endl;
                }
                else if(event.type == SDL_MOUSEBUTTONDOWN) {
                    if(cursorState == 0 || cursorState == 1) cursorState += 2;
                    clicked = true;
                }
                else if(event.type == SDL_MOUSEBUTTONUP) {
                    if(cursorState == 2 || cursorState == 3) cursorState -= 2;
                }
            }
            //Actual processing
            SDL_GetMouseState(&cursorPosition.x, &cursorPosition.y);
            guiCursorPosition.x = (cursorPosition.x / settings.win.scale / 4);
            guiCursorPosition.y = (cursorPosition.y / settings.win.scale / 4);
            inHitboxes = 0;
            if(guiCursorPosition == *fastButton.getHitBox()) {
                if(cursorState >= 2) fastButton.changePressed();
                else fastButton.changeNormal();
                inHitboxes++;
            }
            if(guiCursorPosition == *playButton.getHitBox()) {
                if(cursorState >= 2) playButton.changePressed();
                else playButton.changeNormal();
                inHitboxes++;
                if(clicked) playButton.action(randFunc);
            }
            if(guiCursorPosition == *nextButton.getHitBox()) {
                if(cursorState >= 2) nextButton.changePressed();
                else nextButton.changeNormal();
                inHitboxes++;
            }
            if(guiCursorPosition == *pauseButton.getHitBox()) {
                if(cursorState >= 2) pauseButton.changePressed();
                else pauseButton.changeNormal();
                inHitboxes++;
            }
            if(guiCursorPosition == *stopButton.getHitBox()) {
                if(cursorState >= 2) stopButton.changePressed();
                else stopButton.changeNormal();
                inHitboxes++;
            }
            if(inHitboxes > 0) {
                if(cursorState == 0) cursorState = 1;
                else if(cursorState == 2) cursorState = 3;
            }
            else {
                if(cursorState == 1) cursorState = 0;
                else if(cursorState == 3) cursorState = 2;
            }
            cursorEntity.setXY(cursorPosition.x, cursorPosition.y);
            cursorEntity.setCurrentFrame(cursor.pointers[cursorState]);
            Window.clear();
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
            //Porgress bar
            Window.renderText(progressBarIfTitle);
            Window.renderText(progressBarIdTitle);
            Window.renderText(progressBarOfTitle);
            Window.renderText(progressBarIeTitle);
            Window.renderGui(progressBarEntity);
            Window.renderGui(progressBarNowEntity);
            Window.renderGui(progressBarNextEntity);
            //Buttons
            Window.renderButton(fastButton);
            Window.renderButton(playButton);
            Window.renderButton(nextButton);
            Window.renderButton(pauseButton);
            Window.renderButton(stopButton);
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