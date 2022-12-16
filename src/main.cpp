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
    bool fullInstruction = false, constantFullInstruction = false, progressBarAll = false;
    Uint8 key; //For keyboard input

    //Interpreter
    SystemBus SB;
    CentralMemory CM(&SB, settings.interpreter.ramSize);
    InputOutputDevices IOD(&SB);
    CentralProcessingUnit CPU(&SB, &CM, &IOD, settings.interpreter);
    CM.loadProgram(settings.interpreter.file, settings.interpreter.binary, &logger);
    IOD.input(0x0);

    //Capturing cout in log file
    if(settings.console.log) freopen("log.txt", "w", stdout);
    logger.setColors(settings.console);

    //SDL and IMG initialization
    cout << logger.getStringTime();
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) > 0) {
        cout << logger.error << "SDL Initialization FAILED! SDL_ERROR: " << SDL_GetError() << logger.reset << endl;
    }
    else {
        cout << logger.success << "SDL Initialized Succesfully" << logger.reset << endl;
    }
    cout << logger.getStringTime();
    if(!IMG_Init(IMG_INIT_PNG)) {
        cout << logger.error << "IMG Initialization FAILED! IMG_ERROR: " << SDL_GetError() << logger.reset << endl;
    }
    else {
        cout << logger.success << "IMG Initialized Succesfully" << logger.reset << endl;
    }
    //Printing the settings
    cout << logger.getStringTime() << logger.info << "Settings:" << endl << settings << logger.reset << endl;
    //Render the window
    RenderWindow window("RISC-CPU SIMULATOR v1.0.3", settings.win.width, settings.win.height,
                        flags, &logger, &settings, "res/icon-64.png");
    SDL_ShowCursor(0);

    //Loading the textures
    SDL_Texture* cursorTexture = window.loadTexture("res/cursor.png");
    SDL_Texture* iconTexture = window.loadTexture("res/icon.png");
    SDL_Texture* fontTexture = window.loadTexture("res/font.png");
    SDL_Texture* cpuGuiTexture = window.loadTexture("res/cpu_gui.png");
    SDL_Texture* cmGuiTexture = window.loadTexture("res/cm_gui.png");
    SDL_Texture* iodGuiTexture = window.loadTexture("res/iod_gui.png");
    SDL_Texture* iodKeyTexture = window.loadTexture("res/key.png");
    SDL_Texture* iodKeyPressedTexture = window.loadTexture("res/key_pressed.png");
    SDL_Texture* sbGuiTexture = window.loadTexture("res/system_bus_gui.png");
    SDL_Texture* progressBarTexture = window.loadTexture("res/progress_bar.png");
    SDL_Texture* progressBarNowTexture = window.loadTexture("res/progress_bar_now.png");
    SDL_Texture* progressBarNextTexture = window.loadTexture("res/progress_bar_next.png");
    SDL_Texture* progressBarAllTexture = window.loadTexture("res/progress_bar_all.png");
    SDL_Texture* fastTexture = window.loadTexture("res/fast_button.png");
    SDL_Texture* playTexture = window.loadTexture("res/play_button.png");
    SDL_Texture* nextTexture = window.loadTexture("res/next_button.png");
    SDL_Texture* pauseTexture = window.loadTexture("res/pause_button.png");
    SDL_Texture* reloadTexture = window.loadTexture("res/reload_button.png");
    SDL_Texture* fastPressedTexture = window.loadTexture("res/fast_button_pressed.png");
    SDL_Texture* playPressedTexture = window.loadTexture("res/play_button_pressed.png");
    SDL_Texture* nextPressedTexture = window.loadTexture("res/next_button_pressed.png");
    SDL_Texture* pausePressedTexture = window.loadTexture("res/pause_button_pressed.png");
    SDL_Texture* reloadPressedTexture = window.loadTexture("res/reload_button_pressed.png");
    Entity cursorEntity(Vector2f(0, 0), cursorTexture);
    TextEntity fpsCounterEntity(Vector2f(3, 3), fontTexture, &font);
    //GUI backgrounds
    Entity cpuGui(Vector2f(6, 10), cpuGuiTexture, 256, 128);
    Entity cmGui(Vector2f(70, 10), cmGuiTexture, 256, 128);
    Entity iodGui(Vector2f(111, 51), iodGuiTexture, 256, 128);
    Entity sbGui(Vector2f(6, 121), sbGuiTexture, 128, 384);
    //Instruction name
    TextEntity instNameTitle(Vector2f(32, 3), fontTexture, &font);
    TextEntity instNameValue(Vector2f(82, 3), fontTexture, &font);
    instNameTitle = "Instruction name:";
    instNameValue = "-----";
    //Progress bar
    TextEntity progressBarIfTitle(Vector2f(117, 1), fontTexture, &font);
    TextEntity progressBarIdTitle(Vector2f(125, 1), fontTexture, &font);
    TextEntity progressBarOfTitle(Vector2f(133, 1), fontTexture, &font);
    TextEntity progressBarIeTitle(Vector2f(141, 1), fontTexture, &font);
    Entity progressBarEntity(Vector2f(117, 4), progressBarTexture, 16, 64);
    Entity progressBarNowEntity(Vector2f(117, 4), progressBarNowTexture);
    Entity progressBarNextEntity(Vector2f(125, 4), progressBarNextTexture);
    Entity progressBarAllEntity(Vector2f(117, 4), progressBarAllTexture, 16, 64);
    //Buttons
    Button fastButton(Vector2f(111, 12), HitBox2d(111, 12, 7, 7), fastTexture, fastPressedTexture);
    Button playButton(Vector2f(120, 12), HitBox2d(120, 12, 7, 7), playTexture, playPressedTexture);
    Button nextButton(Vector2f(129, 12), HitBox2d(129, 12, 7, 7), nextTexture, nextPressedTexture);
    Button pauseButton(Vector2f(138, 12), HitBox2d(138, 12, 7, 7), pauseTexture, pausePressedTexture);
    Button reloadButton(Vector2f(147, 12), HitBox2d(147, 12, 7, 7), reloadTexture, reloadPressedTexture);
    fpsCounter = fpsText + fpsString;
    fpsCounterEntity = fpsCounter;
    //Icon
    Entity iconEntity(Vector2f(157, 2), iconTexture, 32, 32);
    TextEntity creditsText0(Vector2f(111, 26), fontTexture, &font);
    TextEntity creditsText1(Vector2f(111, 32), fontTexture, &font);
    TextEntity creditsText2(Vector2f(111, 38), fontTexture, &font);
    creditsText0 = "Reduced Instruction";
    creditsText1 = "Set Computer";
    creditsText2 = "Made by Pyrix25633";
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
    vector<TextEntity> cellTitles;
    vector<TextEntity> cellValues;
    for(Uint16 i = 0; i <= 0xF; i++) {
        cellTitles.push_back(TextEntity(Vector2f(71, 26 + 6 * i), fontTexture, &font));
        cellValues.push_back(TextEntity(Vector2f(91, 26 + 6 * i), fontTexture, &font));
        cellTitles[i] = "0x" + math::Uint16ToHexstr(i) + ":";
        cellValues[i] = "0x00";
    }
    cmTitle = "CM";
    ramTitle = "RAM";
    //IOD
    TextEntity iodTitle(Vector2f(112, 53), fontTexture, &font);
    TextEntity monitorTitle(Vector2f(112, 60), fontTexture, &font);
    TextEntity monitorLine0(Vector2f(112, 67), fontTexture, &font);
    TextEntity monitorLine1(Vector2f(112, 72), fontTexture, &font);
    TextEntity monitorLine2(Vector2f(112, 77), fontTexture, &font);
    TextEntity monitorLine3(Vector2f(112, 82), fontTexture, &font);
    TextEntity keyboardTitle(Vector2f(112, 89), fontTexture, &font);
    vector<Entity> iodKeyEntities;
    vector<TextEntity> iodKeyValues;
    {vector<string> l0 = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
        l1 = {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"},
        l2 = {"a", "s", "d", "f", "g", "h", "j", "k", "l"},
        l3 = {"z", "x", "c", "v", "b", "n", "m"};
    for(Uint8 i = 0; i < 10; i++) {
        iodKeyEntities.push_back(Entity(Vector2f(112 + (6 * i), 96), iodKeyTexture, 12, 12));
        iodKeyValues.push_back(TextEntity(Vector2f(113 + (6 * i), 97), fontTexture, &font));
        iodKeyValues[i] = l0[i];
    }
    for(Uint8 i = 0; i < 10; i++) {
        iodKeyEntities.push_back(Entity(Vector2f(113 + (6 * i), 102), iodKeyTexture, 12, 12));
        iodKeyValues.push_back(TextEntity(Vector2f(114 + (6 * i), 103), fontTexture, &font));
        iodKeyValues[i + 10] = l1[i];
    }
    for(Uint8 i = 0; i < 9; i++) {
        iodKeyEntities.push_back(Entity(Vector2f(114 + (6 * i), 108), iodKeyTexture, 12, 12));
        iodKeyValues.push_back(TextEntity(Vector2f(115 + (6 * i), 109), fontTexture, &font));
        iodKeyValues[i + 20] = l2[i];
    }
    for(Uint8 i = 0; i < 7; i++) {
        iodKeyEntities.push_back(Entity(Vector2f(117 + (6 * i), 114), iodKeyTexture, 12, 12));
        iodKeyValues.push_back(TextEntity(Vector2f(118 + (6 * i), 115), fontTexture, &font));
        iodKeyValues[i + 29] = l3[i];
    }}
    iodTitle = "IOD";
    monitorTitle = "0x0000 Monitor";
    monitorLine0 = "";
    monitorLine1 = "";
    monitorLine2 = "";
    monitorLine3 = "";
    keyboardTitle = "0x0001 Keyboard";
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
    cbValue = "DWB";

    //Running
    previousSecond = SDL_GetTicks() / 1000;
    while(running) {
        window.calculateScale();
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
                SDL_Scancode code = event.key.keysym.scancode;
                switch(event.type) {
                    case SDL_QUIT:
                        running = false;
                        cout << logger.getStringTime() << logger.info << "Windwow Closed" << logger.reset << endl;
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
                            if(event.wheel.y < 0) {
                                if(cellsStartAddress < settings.interpreter.ramSize - 4)
                                    cellsStartAddress += 4;
                                else
                                    cellsStartAddress = 4 - (settings.interpreter.ramSize - cellsStartAddress);
                            }
                            else {
                                if(cellsStartAddress >= 4)
                                    cellsStartAddress -= 4;
                                else
                                    cellsStartAddress = settings.interpreter.ramSize - (4 - cellsStartAddress);
                            }
                        break;
                    case SDL_KEYDOWN:
                        if(code >= SDL_SCANCODE_A && code <= SDL_SCANCODE_Z) {
                            key = code + 93;
                        }
                        else if(code >= SDL_SCANCODE_1 && code <= SDL_SCANCODE_9) {
                            key = code + 19;
                        }
                        else if(code >= SDL_SCANCODE_KP_1 && code <= SDL_SCANCODE_KP_9) {
                            key = code - 40;
                        }
                        else if(code == SDL_SCANCODE_0 || code == SDL_SCANCODE_KP_0) {
                            key = '0';
                        }
                        else if(code == SDL_SCANCODE_KP_ENTER) {
                            key = '\r';
                        }
                        else if(code == SDL_SCANCODE_KP_SPACE) {
                            key = ' ';
                        }
                        IOD.input(key);
                        break;
                    case SDL_KEYUP:
                        key = 0x0;
                        IOD.input(key);
                        break;
                }
            }
            //Actual processing
            SDL_GetMouseState(&cursorPosition.x, &cursorPosition.y);
            guiCursorPosition.x = (cursorPosition.x / settings.win.scale / window.getScale());
            guiCursorPosition.y = (cursorPosition.y / settings.win.scale / window.getScale());
            inHitboxes = 0;
            if(guiCursorPosition == *fastButton.getHitBox()) {
                if(cursorState >= 2) fastButton.changePressed();
                else fastButton.changeNormal();
                inHitboxes++;
                if(clicked) {
                    constantRefresh = true;
                    constantFullInstruction = true;
                }
            }
            if(guiCursorPosition == *playButton.getHitBox()) {
                if(cursorState >= 2) playButton.changePressed();
                else playButton.changeNormal();
                inHitboxes++;
                if(clicked) {
                    refresh = true;
                    fullInstruction = true;
                }
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
                    progressBarAll = false;
                }
            }
            if(guiCursorPosition == *pauseButton.getHitBox()) {
                if(cursorState >= 2) pauseButton.changePressed();
                else pauseButton.changeNormal();
                inHitboxes++;
                if(clicked) {
                    constantRefresh = false;
                    refresh = true;
                    constantFullInstruction = false;
                }
            }
            if(guiCursorPosition == *reloadButton.getHitBox()) {
                if(cursorState >= 2) reloadButton.changePressed();
                else reloadButton.changeNormal();
                inHitboxes++;
                if(clicked) {
                    constantRefresh = false;
                    refresh = true;
                    constantFullInstruction = false;
                    settings = JsonManager::getSettings();
                    CPU.reset(settings.interpreter);
                    SB = SystemBus();
                    CM.reset(settings.interpreter.ramSize);
                    IOD.reset();
                    CM.loadProgram(settings.interpreter.file, settings.interpreter.binary, &logger);
                    msStep = 1000 / settings.win.maxFps;
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
            window.clear();
            if(settings.win.fpsCounter) {
                window.renderText(fpsCounterEntity);
            }
            
            instNameValue = CPU.getInstName();
            if(phaseNext >= 4) {
                fullInstruction = false;
                constantFullInstruction = false;
            }
            CPU.getPhases(phaseNow, phaseNext);
            if(fullInstruction || constantFullInstruction) {
                switch(phaseNext) {
                    case 0: CPU.fetchInstruction();
                    case 1: CPU.decodeInstruction();
                    case 2: 
                        CPU.getPhases(phaseNow, phaseNext);
                        if(phaseNext == 2) CPU.fetchOperand();
                    case 3: CPU.executeInstruction();
                }
                fullInstruction = false;
                progressBarAll = true;
            }
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
                    if(j == settings.interpreter.ramSize) j = 0;
                    cellTitles[i] = "0x" + math::Uint16ToHexstr(j) + ":";
                    cellValues[i] = "0x" + math::Uint8ToHexstr(CM.get(j));
                }
                CPU.getPhases(phaseNow, phaseNext);
                string l0, l1, l2, l3;
                IOD.getLines(l0, l1, l2, l3);
                monitorLine0 = l0;
                monitorLine1 = l1;
                monitorLine2 = l2;
                monitorLine3 = l3;
                abValue = "0x" + math::Uint16ToHexstr(SB.getAddress());
                dbValue = "0x" + math::Uint16ToHexstr(SB.getData());
                cbValue = math::ControlBusToHexstr(SB.getControl());
                progressBarNowEntity.setX(117 + 8 * phaseNow);
                progressBarNextEntity.setX(117 + 8 * phaseNext);
            }
            if(key != 0x0) {
                Uint8 indexKey;
                switch(key) {
                    case '1': indexKey = 0; break;
                    case '2': indexKey = 1; break;
                    case '3': indexKey = 2; break;
                    case '4': indexKey = 3; break;
                    case '5': indexKey = 4; break;
                    case '6': indexKey = 5; break;
                    case '7': indexKey = 6; break;
                    case '8': indexKey = 7; break;
                    case '9': indexKey = 8; break;
                    case '0': indexKey = 9; break;
                    case 'q': indexKey = 10; break;
                    case 'w': indexKey = 11; break;
                    case 'e': indexKey = 12; break;
                    case 'r': indexKey = 13; break;
                    case 't': indexKey = 14; break;
                    case 'y': indexKey = 15; break;
                    case 'u': indexKey = 16; break;
                    case 'i': indexKey = 17; break;
                    case 'o': indexKey = 18; break;
                    case 'p': indexKey = 19; break;
                    case 'a': indexKey = 20; break;
                    case 's': indexKey = 21; break;
                    case 'd': indexKey = 22; break;
                    case 'f': indexKey = 23; break;
                    case 'g': indexKey = 24; break;
                    case 'h': indexKey = 25; break;
                    case 'j': indexKey = 26; break;
                    case 'k': indexKey = 27; break;
                    case 'l': indexKey = 28; break;
                    case 'z': indexKey = 29; break;
                    case 'x': indexKey = 30; break;
                    case 'c': indexKey = 31; break;
                    case 'v': indexKey = 32; break;
                    case 'b': indexKey = 33; break;
                    case 'n': indexKey = 34; break;
                    case 'm': indexKey = 35; break;
                }
                for(Uint8 i = 0; i < 36; i++) {
                    iodKeyEntities[i].setTexture(((i == indexKey) ? iodKeyPressedTexture : iodKeyTexture));
                }
            }
            else {
                for(Entity &e : iodKeyEntities) {
                    e.setTexture(iodKeyTexture);
                }
            }
            //GUI backgrounds
            window.renderGui(cpuGui);
            window.renderGui(cmGui);
            window.renderGui(iodGui);
            window.renderGui(sbGui);
            //CPU Render
            window.renderText(cpuTitle);
            window.renderText(cuTitle);
            window.renderText(aluTitle);
            window.renderText(pcTitle);
            window.renderText(irTitle);
            window.renderText(srTitle);
            window.renderText(srElements);
            window.renderText(arTitle);
            window.renderText(drTitle);
            window.renderText(spTitle);
            window.renderText(pcValue);
            window.renderText(irValue);
            window.renderText(srValue);
            window.renderText(arValue);
            window.renderText(drValue);
            window.renderText(spValue);
            for(TextEntity &e : registriesTitles) {
                window.renderText(e);
            }
            for(TextEntity &e : registriesValues) {
                window.renderText(e);
            }
            //CM Render
            window.renderText(cmTitle);
            window.renderText(ramTitle);
            for(TextEntity &e : cellTitles) {
                window.renderText(e);
            }
            for(TextEntity &e : cellValues) {
                window.renderText(e);
            }
            //IOD Render
            window.renderText(iodTitle);
            window.renderText(monitorTitle);
            window.renderText(monitorLine0);
            window.renderText(monitorLine1);
            window.renderText(monitorLine2);
            window.renderText(monitorLine3);
            window.renderText(keyboardTitle);
            for(Entity &e : iodKeyEntities) {
                window.renderGui(e);
            }
            for(TextEntity &e : iodKeyValues) {
                window.renderText(e);
            }
            //SB Render
            window.renderText(abTitle);
            window.renderText(dbTitle);
            window.renderText(cbTitle);
            window.renderText(abValue);
            window.renderText(dbValue);
            window.renderText(cbValue);
            //Instruction name
            window.renderText(instNameTitle);
            window.renderText(instNameValue);
            //Progress bar
            window.renderText(progressBarIfTitle);
            window.renderText(progressBarIdTitle);
            window.renderText(progressBarOfTitle);
            window.renderText(progressBarIeTitle);
            window.renderGui(progressBarEntity);
            if(phaseNow < 4 && !progressBarAll) {
                window.renderGui(progressBarNowEntity);
            }
            if(phaseNext < 4 && !progressBarAll) {
                window.renderGui(progressBarNextEntity);
            }
            if(progressBarAll) {
                window.renderGui(progressBarAllEntity);
            }
            //Buttons
            window.renderButton(fastButton);
            window.renderButton(playButton);
            window.renderButton(nextButton);
            window.renderButton(pauseButton);
            window.renderButton(reloadButton);
            //Icon
            window.renderGui(iconEntity);
            window.renderText(creditsText0);
            window.renderText(creditsText1);
            window.renderText(creditsText2);
            //Display
            window.renderCursor(cursorEntity);
            window.display();
        }
    }
    //Quitting window
    window.cleanUp();
    SDL_Quit();
    return 0;
}

//For Windows OS
int WinMain(int argc, char* args[]) {
    return main(argc, args);
}