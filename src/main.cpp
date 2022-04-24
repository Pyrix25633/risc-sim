#include <iostream>
#include <vector>
#include <cstdio>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_audio.h>

#include "renderwindow.hpp"
#include "entity.hpp"
#include "utils.hpp"
#include "risc.hpp"

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
    Uint8 cursorState = 0; //0 -> normal, 1 -> hover, 2 -> normal clicked, 3 -> hover clicked
    Cursor cursor = JsonManager::getCursor();
    Uint8 inHitboxes = 0, phaseNow, phaseNext;
    Uint16 cellsStartAddress = 0x0; //For CM rendering
    bool clicked = false, refresh = true, constantRefresh = false;

    //Interpreter
    SystemBus SB;
    CentralMemory CM(&SB, settings.interpreter.ramSize);
    InputOutputDevices IOD(&SB);
    CentralProcessingUnit CPU(&SB, &CM, &IOD, settings.interpreter.start);
    CM.loadProgram(settings.interpreter.file, settings.interpreter.binary, &logger);

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
    RenderWindow Window("RISC-CPU SIMULATOR v0.1.0", settings.win.width, settings.win.height
        , flags, &logger, &settings, "res/img/icon.png");
    SDL_ShowCursor(0);

    //Loading the textures
    SDL_Texture* cursorTexture = Window.loadTexture("res/img/cursor.png");
    SDL_Texture* fontTexture = Window.loadTexture("res/img/font.png");
    SDL_Texture* cpuGuiTexture = Window.loadTexture("res/img/cpu_gui.png");
    SDL_Texture* cmGuiTexture = Window.loadTexture("res/img/cm_gui.png");
    SDL_Texture* sbGuiTexture = Window.loadTexture("res/img/system_bus_gui.png");
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
    TextEntity fpsCounterEntity(Vector2f(3, 3), fontTexture, &font);
    //GUI backgrounds
    Entity cpuGui(Vector2f(6, 10), cpuGuiTexture, 256, 128);
    Entity cmGui(Vector2f(70, 10), cmGuiTexture, 256, 128);
    Entity sbGui(Vector2f(6, 121), sbGuiTexture, 128, 256);
    //Instruction name
    TextEntity instNameTitle(Vector2f(32, 3), fontTexture, &font);
    TextEntity instNameValue(Vector2f(84, 3), fontTexture, &font);
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
    TextEntity cpuTitle(Vector2f(7, 12), fontTexture, &font);
    TextEntity cuTitle(Vector2f(7, 19), fontTexture, &font);
    TextEntity aluTitle(Vector2f(36, 19), fontTexture, &font);
    TextEntity pcTitle(Vector2f(7, 26), fontTexture, &font);
    TextEntity irTitle(Vector2f(7, 34), fontTexture, &font);
    TextEntity srTitle(Vector2f(7, 47), fontTexture, &font);
    TextEntity srElements(Vector2f(15, 42), fontTexture, &font);
    TextEntity arTitle(Vector2f(7, 55), fontTexture, &font);
    TextEntity drTitle(Vector2f(7, 63), fontTexture, &font);
    TextEntity spTitle(Vector2f(7, 71), fontTexture, &font);
    TextEntity pcValue(Vector2f(15, 26), fontTexture, &font);
    TextEntity irValue(Vector2f(15, 34), fontTexture, &font);
    TextEntity srValue(Vector2f(15, 47), fontTexture, &font);
    TextEntity arValue(Vector2f(15, 55), fontTexture, &font);
    TextEntity drValue(Vector2f(15, 63), fontTexture, &font);
    TextEntity spValue(Vector2f(15, 71), fontTexture, &font);
    vector<TextEntity> registriesTitles;
    vector<TextEntity> registriesValues;
    for(Uint8 i = 0; i <= 0xF; i++) {
        registriesTitles.push_back(TextEntity(Vector2f(36, 26 + 6 * i), fontTexture, &font));
        registriesValues.push_back(TextEntity(Vector2f(44, 26 + 6 * i), fontTexture, &font));
        string s = "!";
        s[0] = ((i < 0xA) ? (i + 48) : (i + 55));
        registriesTitles[i] = "R" + s + ":";
        registriesValues[i] = "0x0000";
    }
    instNameTitle = "Instruction name:";
    instNameValue = "-----";
    progressBarIfTitle = "IF";
    progressBarIdTitle = "ID";
    progressBarOfTitle = "OF";
    progressBarIeTitle = "IE";
    cpuTitle = "CPU";
    cuTitle = "CU";
    aluTitle = "ALU";
    pcTitle = "PC:";
    irTitle = "IR:";
    srTitle = "SR:";
    srElements = "ZNCV";
    arTitle = "AR:";
    drTitle = "DR:";
    spTitle = "SP:";
    pcValue = "0x0000";
    irValue = "0x0000";
    srValue = "0000";
    arValue = "0x0000";
    drValue = "0x0000";
    spValue = "0x0000";
    //CM
    TextEntity cmTitle(Vector2f(71, 12), fontTexture, &font);
    TextEntity ramTitle(Vector2f(71, 19), fontTexture, &font);
    vector<TextEntity> cellsTitles;
    vector<TextEntity> cellsValues;
    for(Uint16 i = 0; i <= 0xF; i++) {
        cellsTitles.push_back(TextEntity(Vector2f(71, 26 + 6 * i), fontTexture, &font));
        cellsValues.push_back(TextEntity(Vector2f(91, 26 + 6 * i), fontTexture, &font));
        cellsTitles[i] = "0x" + math::Uint16ToHexstr(i) + ":";
        cellsValues[i] = "0x00";
    }
    cmTitle = "CM";
    ramTitle = "RAM";
    //SB
    TextEntity abTitle(Vector2f(42, 134), fontTexture, &font);
    TextEntity dbTitle(Vector2f(42, 145), fontTexture, &font);
    TextEntity cbTitle(Vector2f(42, 156), fontTexture, &font);
    TextEntity abValue(Vector2f(50, 134), fontTexture, &font);
    TextEntity dbValue(Vector2f(50, 145), fontTexture, &font);
    TextEntity cbValue(Vector2f(50, 156), fontTexture, &font);
    abTitle = "AB:";
    dbTitle = "DB:";
    cbTitle = "CB:";
    abValue = "0x0000";
    dbValue = "0x0000";
    cbValue = "0x0000";

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
                switch(event.type) {
                    case SDL_QUIT:
                        running = false;
                        cout << logger.getStringTime() << logger.info << "Game Windwow Closed" << logger.reset << endl;
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        if(cursorState == 0 || cursorState == 1) cursorState += 2;
                        clicked = true;
                        break;
                    case SDL_MOUSEBUTTONUP:
                        if(cursorState == 2 || cursorState == 3) cursorState -= 2;
                        break;
                    case SDL_MOUSEWHEEL:
                            refresh = true;
                            cellsStartAddress += 4 * event.wheel.y * -1;
                        break;
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
                if(clicked) constantRefresh = true;
            }
            if(guiCursorPosition == *playButton.getHitBox()) {
                if(cursorState >= 2) playButton.changePressed();
                else playButton.changeNormal();
                inHitboxes++;
                if(clicked) refresh = true;
            }
            if(guiCursorPosition == *nextButton.getHitBox()) {
                if(cursorState >= 2) nextButton.changePressed();
                else nextButton.changeNormal();
                inHitboxes++;
                if(clicked) {
                    refresh = true;
                    switch(phaseNext) {
                        case 0:
                            CPU.fetchInstruction();
                            break;
                        case 1:
                            CPU.decodeInstruction();
                            break;
                        case 2:
                            CPU.fetchOperand();
                            break;
                        case 3:
                            CPU.executeInstruction();
                    }
                }
            }
            if(guiCursorPosition == *pauseButton.getHitBox()) {
                if(cursorState >= 2) pauseButton.changePressed();
                else pauseButton.changeNormal();
                inHitboxes++;
                if(clicked) {
                    constantRefresh = false;
                    refresh = true;
                }
            }
            if(guiCursorPosition == *stopButton.getHitBox()) {
                if(cursorState >= 2) stopButton.changePressed();
                else stopButton.changeNormal();
                inHitboxes++;
                if(clicked) {
                    constantRefresh = false;
                    refresh = true;
                }
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
            
            instNameValue = CPU.getInstName();
            if(refresh || constantRefresh) {
                //CPU Values
                pcValue = "0x" + math::Uint16ToHexstr(CPU.getPC());
                irValue = "0x" + math::Uint16ToHexstr(CPU.getIR());
                srValue = math::StatusRegisterToHexstr(CPU.getSR());
                arValue = "0x" + math::Uint16ToHexstr(CPU.getAR());
                drValue = "0x" + math::Uint16ToHexstr(CPU.getDR());
                spValue = "0x" + math::Uint16ToHexstr(CPU.getSP());
                refresh = false;
                for(Uint8 i = 0; i < 16; i++) {
                    registriesValues[i] = "0x" + math::Uint16ToHexstr(CPU.getR(i));
                }
                for(Uint16 i = 0, j = cellsStartAddress; i < 16; j++, i++) {
                    cellsTitles[i] = "0x" + math::Uint16ToHexstr(j) + ":";
                    cellsValues[i] = "0x" + math::Uint8ToHexstr(CM.get(j));
                }
                CPU.getPhases(phaseNow, phaseNext);
                progressBarNowEntity.setX(178 + 8 * phaseNow);
                progressBarNextEntity.setX(178 + 8 * phaseNext);
                abValue = "0x" + math::Uint16ToHexstr(SB.getAddress());
                dbValue = "0x" + math::Uint16ToHexstr(SB.getData());
                cbValue = "0x" + math::ControlBusToHexstr(SB.getControl());
            }
            //GUI backgrounds
            Window.renderGui(cpuGui);
            Window.renderGui(cmGui);
            Window.renderGui(sbGui);
            //CPU Render
            Window.renderText(cpuTitle);
            Window.renderText(cuTitle);
            Window.renderText(aluTitle);
            Window.renderText(pcTitle);
            Window.renderText(irTitle);
            Window.renderText(srTitle);
            Window.renderText(srElements);
            Window.renderText(arTitle);
            Window.renderText(drTitle);
            Window.renderText(spTitle);
            Window.renderText(pcValue);
            Window.renderText(irValue);
            Window.renderText(srValue);
            Window.renderText(arValue);
            Window.renderText(drValue);
            Window.renderText(spValue);
            for(TextEntity e : registriesTitles) {
                Window.renderText(e);
            }
            for(TextEntity e : registriesValues) {
                Window.renderText(e);
            }
            //CM Render
            Window.renderText(cmTitle);
            Window.renderText(ramTitle);
            for(TextEntity e : cellsTitles) {
                Window.renderText(e);
            }
            for(TextEntity e : cellsValues) {
                Window.renderText(e);
            }
            //SB Render
            Window.renderText(abTitle);
            Window.renderText(dbTitle);
            Window.renderText(cbTitle);
            Window.renderText(abValue);
            Window.renderText(dbValue);
            Window.renderText(cbValue);
            //Instruction name
            Window.renderText(instNameTitle);
            Window.renderText(instNameValue);
            //Progress bar
            Window.renderText(progressBarIfTitle);
            Window.renderText(progressBarIdTitle);
            Window.renderText(progressBarOfTitle);
            Window.renderText(progressBarIeTitle);
            Window.renderGui(progressBarEntity);
            if(phaseNow < 4) {
                Window.renderGui(progressBarNowEntity);
            }
            if(phaseNext < 4) {
                Window.renderGui(progressBarNextEntity);
            }
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