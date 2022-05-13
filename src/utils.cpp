#include "utils.hpp"
#include "math.h"

using namespace std;
using namespace Json;

string Logger::getStringTime() {
    time_t timet = time(0);
    tm *timei = localtime(&timet);
    int day = timei->tm_mday, month = timei->tm_mon + 1, year = timei->tm_year + 1900,
        hour = timei->tm_hour, minute = timei->tm_min, second = timei->tm_sec, tick = SDL_GetTicks() % 1000;
    string s = "[";
    if(day < 10) s += '0';
    s += to_string(day) + '/';
    if(month < 10) s+= '0';
    s += to_string(month) + '/' + to_string(year) + ' ';
    if(hour < 10) s += '0';
    s += to_string(hour) + ':';
    if(minute < 10) s += '0';
    s += to_string(minute) + ':';
    if(second < 10) s += '0';
    s += to_string(second) + '.';
    if(tick < 100) s += '0';
    if(tick < 10) s += '0';
    s += to_string(tick) + "] ";
    return s;
}

void Logger::setColors(ConsoleSettings c) {
    if(c.log) {
        reset = "";
        success = "[Success] ";
        error = "[Error] ";
        warning = "[Warning] ";
        info = "[Info] ";
    }
    else {
        if(c.color) {
            reset = "\033[0m";
            success = "\033[32m";
            error = "\033[31m";
            warning = "\033[33m";
            info = "\033[34m";
        }
        else {
            reset = "";
            success = "[Success] ";
            error = "[Error] ";
            warning = "[Warning] ";
            info = "[Info] ";
        }
    }
}

float Logger::getSecond() {
    return SDL_GetTicks() / 1000;
}

ostream& operator << (ostream& os, const Settings& settings) {
    return os << "Window Height: " << settings.win.height << endl
            << "Window Width: " << settings.win.width << endl
            << "Max Framerate: " << settings.win.maxFps << endl
            << "Scale: " << settings.win.scale << endl
            << "Gui Scale: " << settings.win.guiScale << endl
            << "Vsync: " << ((settings.win.flags.vsync) ? "true" : "false") << endl
            << "GPU Acceleration: " << ((settings.win.flags.gpuAcc) ? "true" : "false") << endl
            << "Software Fallback: " << ((settings.win.flags.software) ? "true" : "false") << endl
            << "Target Texture: " << ((settings.win.flags.targTex) ? "true" : "false") << endl
            << "FPS Counter: " << ((settings.win.fpsCounter) ? "true" : "false") << endl
            << "Log File: " << ((settings.console.log) ? "true" : "false") << endl
            << "Output Color: " << ((settings.console.color) ? "true" : "false") << endl
            << "Interpreter File: " << settings.interpreter.file
            << " Type: " << ((settings.interpreter.binary) ? "binary" : "hexadecimal") << endl
            << "Interpreter Ram Size: " << settings.interpreter.ramSize << endl
            << "Interpreter Start Address: " << settings.interpreter.start;
}

Settings JsonManager::getSettings() {
    Settings settings;
    Uint8 errors = 0;
    ifstream file("settings/settings.json");
    Value actualJson, window, rendererFlags, interpreter, console;
    Reader reader;
    reader.parse(file, actualJson);
    window = actualJson["window"];
    rendererFlags = window["renderer_flags"];
    interpreter = actualJson["interpreter"];
    console = actualJson["console"];
    settings.win.height = window["height"].asInt();
    if(!file) errors++;
    if(settings.win.height == 0) {
        window["height"] = 500;
        settings.win.height = 500;
        errors++;
    }
    settings.win.width = window["width"].asInt();
    if(settings.win.width == 0) {
        window["width"] = 1000;
        settings.win.width = 1000;
        errors++;
    }
    settings.win.maxFps = window["max_framerate"].asInt();
    if(settings.win.maxFps == 0) {
        window["max_framerate"] = 60;
        settings.win.maxFps = 60;
        errors++;
    }
    settings.win.scale = window["scale"].asFloat();
    if(settings.win.scale < 0.5 || settings.win.scale > 3) {
        window["scale"] = 1.0;
        settings.win.scale = 1.0;
        errors++;
    }
    settings.win.guiScale = window["gui_scale"].asFloat();
    if(settings.win.guiScale < 0.5 || settings.win.guiScale > 3) {
        window["gui_scale"] = 1.0;
        settings.win.guiScale = 1.0;
        errors++;
    }
    settings.win.flags.vsync = rendererFlags["vsync"].asBool();
    settings.win.flags.gpuAcc = rendererFlags["gpu_acceleration"].asBool();
    settings.win.flags.software = rendererFlags["software_fallback"].asBool();
    settings.win.flags.targTex = rendererFlags["target_texture"].asBool();
    settings.win.fpsCounter = window["fps_counter"].asBool();
    settings.console.log = console["file_log"].asBool();
    settings.console.color = console["output_color"].asBool();
    settings.interpreter.file = interpreter["file"].asString();
    if(settings.interpreter.file == "") {
        settings.interpreter.file = "binary.bin";
        interpreter["file"] = "binary.bin";
        errors++;
    }
    settings.interpreter.ramSize = interpreter["ram_size"].asInt();
    if(settings.interpreter.ramSize > 0x10000) {
        settings.interpreter.ramSize = 0x10000;
        interpreter["ram_size"] = 0x10000;
        errors++;
    }
    settings.interpreter.start = interpreter["start"].asInt();
    if(settings.interpreter.start > 0xFFFF) {
        settings.interpreter.start = 0x0000;
        interpreter["start"] = 0x0000;
        errors++;
    }
    string binFile = settings.interpreter.file;
    Uint16 lenght = binFile.length();
    settings.interpreter.binary = (binFile[lenght - 4] == '.' && binFile[lenght - 3] == 'b'
        && binFile[lenght - 2] == 'i' && binFile[lenght - 1] == 'n');
    file.close();
    if(errors > 0) {
        ofstream outFile("settings/settings.json");
        window["renderer_flags"] = rendererFlags;
        actualJson["window"] = window;
        actualJson["interpreter"] = interpreter;
        actualJson["console"] = console;
        outFile << actualJson;
        cout << "[Warning] " << "The settings file was not found or there were some errors in it, so it has been rewritten." << endl;
    }
    return settings;
}

Uint32 JsonManager::getFlags(Settings settings) {
    Uint32 flags = 0;
    if(settings.win.flags.vsync) {
        flags |= SDL_RENDERER_PRESENTVSYNC;
    }
    if(settings.win.flags.gpuAcc) {
        flags |= SDL_RENDERER_ACCELERATED;
    }
    if(settings.win.flags.software) {
        flags |= SDL_RENDERER_SOFTWARE;
    }
    if(settings.win.flags.targTex) {
        flags |= SDL_RENDERER_TARGETTEXTURE;
    }
    return flags;
}

Font JsonManager::getFont() {
    Font font;
    ifstream file("res/img/font.json");
    Value actualJson, letterValue;
    Reader reader;
    string str;
    reader.parse(file, actualJson);
    for(Uint16 i = 0; i < 128; i++) {
        str = char(i);
        letterValue = actualJson[str];
        font.letters[i].x = letterValue["x"].asInt();
        font.letters[i].y = letterValue["y"].asInt();
        font.letters[i].w = 8;
        font.letters[i].h = 8;
    }
    return font;
}

Cursor JsonManager::getCursor() {
    Cursor cursor;
    ifstream file("res/img/cursor.json");
    Value actualJson, letterValue;
    Reader reader;
    reader.parse(file, actualJson);
    for(Uint16 i = 0; i < 4; i++) {
        letterValue = actualJson[to_string(i)];
        cursor.pointers[i].x = letterValue["x"].asInt();
        cursor.pointers[i].y = letterValue["y"].asInt();
        cursor.pointers[i].w = 16;
        cursor.pointers[i].h = 16;
    }
    return cursor;
}