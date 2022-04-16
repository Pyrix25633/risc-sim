#include "utils.hpp"

using namespace std;
using namespace Json;

ControlBus::ControlBus() {
    this->reset();
}

ControlBus::ControlBus(bool r, bool m, bool w) :R(r), M(m), W(w) {}

void ControlBus::reset() {
    R = WRITE;
    M = INPUTOUTPUT;
    W = BYTE;
}

Instruction::Instruction() :group(0), addressing(0), opcode(0), r1(0), r2(0), offset(0) {}

StatusRegister::StatusRegister() :Z(false), C(false), N(false), V(false) {}

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
            << "Interpreter File: " << settings.interpreter.file << endl
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
        settings.interpreter.file = "binary.bnr";
        interpreter["file"] = "binary.bin";
        errors++;
    }
    settings.interpreter.ramSize = interpreter["ram_size"].asInt();
    if(settings.interpreter.ramSize > 0xFFFF) {
        settings.interpreter.ramSize = 0x0000;
        interpreter["ram_size"] = 0x0000;
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
    if(binFile[binFile[lenght - 3] == '.' && lenght - 2] == 'b' && binFile[lenght - 1] == 'i' && binFile[lenght] == 'n')
        settings.interpreter.binary = true;
    else
        settings.interpreter.binary = false;
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

SystemBus::SystemBus() :AB(0x0), DB(0x0), CB(ControlBus()) {}

void SystemBus::writeAddress(Uint16 a) {
    AB = a;
}

void SystemBus::writeData(Uint16 d) {
    DB = d;
}

void SystemBus::writeControl(ControlBus c) {
    CB = c;
}

Uint16 SystemBus::readAddress() {
    Uint16 a = AB;
    AB = 0x0;
    return a;
}

Uint16 SystemBus::readData() {
    Uint16 d = DB;
    DB = 0x0;
    return d;
}

ControlBus SystemBus::readControl() {
    ControlBus c = CB;
    CB.R = WRITE;
    CB.M = INPUTOUTPUT;
    CB.W = BYTE;
    return c;
}

Uint16 SystemBus::getAddress() {
    return AB;
}

Uint16 SystemBus::getData() {
    return DB;
}

ControlBus SystemBus::getControl() {
    return CB;
}

void SystemBus::resetBus() {
    AB = 0x0;
    DB = 0x0;
    CB.reset();
}

ArithmeticLogicUnit::ArithmeticLogicUnit(StatusRegister* pSR) :SR(pSR) {
    for(Uint8 i = 0; i < 0xF; i++) {
        R[i] = 0x0000;
    }
}

void ArithmeticLogicUnit::add(Uint8 d, Uint8 s) {
    if(d > 0xF) return;
    if(s > 0xF) return;
    Uint32 res1 = R[d] + R[s];
    Int16 a = Int16(R[d]), b = Int16(R[s]), c = a + b;
    Int32 res2 = a + b;
    R[d] += R[s];
    SR->C = (R[d] != res1);
    SR->V = (c != res2);
    SR->N = (res2 < 0x0);
    SR->Z = (R[d] == 0x0);
}

void ArithmeticLogicUnit::sub(Uint8 d, Uint8 s) {
    if(d > 0xF) return;
    if(s > 0xF) return;
    Uint32 res1 = R[d] - R[s];
    Int16 a = Int16(R[d]), b = Int16(R[s]), c = a - b;
    Int32 res2 = a + b;
    R[d] -= R[s];
    SR->C = (R[d] != res1);
    SR->V = (c != res2);
    SR->N = (res2 < 0x0);
    SR->Z = (R[d] == 0x0);
}

void ArithmeticLogicUnit::bNot(Uint8 r) {
    if(r > 0xF) return;
    R[r] = ~R[r];
    SR->C = false;
    SR->V = false;
    SR->N = (Int16(R[r]) < 0x0);
    SR->Z = (R[r] == 0x0);
}

void ArithmeticLogicUnit::bAnd(Uint8 d, Uint8 s) {
    if(d > 0xF) return;
    if(s > 0xF) return;
    R[d] = R[d] & R[s];
    SR->C = false;
    SR->V = false;
    SR->N = (Int16(R[d]) < 0x0);
    SR->Z = (R[d] == 0x0);
}

void ArithmeticLogicUnit::bOr(Uint8 d, Uint8 s) {
    if(d > 0xF) return;
    if(s > 0xF) return;
    R[d] = R[d] | R[s];
    SR->C = false;
    SR->V = false;
    SR->N = (Int16(R[d]) < 0x0);
    SR->Z = (R[d] == 0x0);
}

void ArithmeticLogicUnit::bXor(Uint8 d, Uint8 s) {
    if(d > 0xF) return;
    if(s > 0xF) return;
    R[d] = R[d] ^ R[s];
    SR->C = false;
    SR->V = false;
    SR->N = (Int16(R[d]) < 0x0);
    SR->Z = (R[d] == 0x0);
}

void ArithmeticLogicUnit::inc(Uint8 r) {
    if(r > 0xF) return;
    SR->C = (R[r] == 0xFFFF);
    SR->V = (R[r] == 0x7FFF);
    R[r]++;
    SR->Z = (R[r] == 0x0);
    SR->N = (Int16(R[r]) < 0x0);
}

void ArithmeticLogicUnit::dec(Uint8 r) {
    if(r > 15) return;
    SR->C = true;
    SR->V = (R[r] == 0x8000);
    R[r]--;
    SR->Z = (R[r] == 0x0);
    SR->N = (Int16(R[r]) < 0x0);
}

void ArithmeticLogicUnit::lShift(Uint8 r) {
    if(r > 15) return;
    SR->C = (R[r] >= 0x8000);    
    R[r] = R[r] << 1;
    SR->V = false;
    SR->Z = (R[r] == 0x0);
    SR->N = (Int16(R[r]) < 0x0);
}

void ArithmeticLogicUnit::rShift(Uint8 r) {
    if(r > 15) return;
    R[r] = R[r] >> 1;
    SR->C = false;
    SR->V = false;
    SR->Z = (R[r] == 0x0);
    SR->N = (Int16(R[r]) < 0x0);
}

void ArithmeticLogicUnit::load(Uint8 r, Uint16 word) {
    if(r > 0xF) return;
    R[r] = word;
}

Uint16 ArithmeticLogicUnit::get(Uint8 r) {
    if(r > 0xF) return 0;
    return R[r];
}

CentralProcessingUnit::CentralProcessingUnit(SystemBus* pSB, CentralMemory* pCM, InputOutputDevices* pIOD, Uint16 start)
    :ALU(ArithmeticLogicUnit(&SR)), SB(pSB), CM(pCM), IOD(pIOD), PC(start) {}

void CentralProcessingUnit::fetchInstruction() {
    AR = PC;
    SB->writeAddress(AR);
    SB->writeControl(ControlBus(READ, MEMORY, WORD));
    CM->operate();
    IR = SB->readData();
    PC += 2;
}

void CentralProcessingUnit::decodeInstruction() {
    I.group = (IR & 0xC000) >> 14;
    I.addressing = (IR & 0x3000) >> 12;
    I.opcode = (IR & 0x0F00) >> 8;
    if(I.group == 0x3) {
        I.offset = (IR & 0x00FF);
    }
    else {
        I.r1 = (IR & 0x00F0) >> 4;
        I.r2 = (IR & 0x000F);
    }
}

Uint16 CentralProcessingUnit::getPC() {
    return PC;
}

Uint16 CentralProcessingUnit::getIR() {
    return IR;
}

StatusRegister CentralProcessingUnit::getSR() {
    return SR;
}

Uint16 CentralProcessingUnit::getAR() {
    return AR;
}

Uint16 CentralProcessingUnit::getDR() {
    return DR;
}

Uint16 CentralProcessingUnit::getSP() {
    return SP;
}

void CentralProcessingUnit::ldwa() {
    Uint16 data;
    AR = PC;
    SB->writeAddress(AR);
    SB->writeControl(ControlBus(READ, MEMORY, WORD));
    CM->operate();
    data = SB->readData();
    ALU.load(I.r1, data);
    SR.Z = (data == 0);
    SR.N = Int16(data) < 0;
    PC += 2;
}

void CentralProcessingUnit::ldwi() {
    Uint16 data;
    AR = PC;
    SB->writeAddress(AR);
    SB->writeControl(ControlBus(READ, MEMORY, WORD));
    CM->operate();
    SB->writeAddress(SB->readData());
    SB->writeControl(ControlBus(READ, MEMORY, WORD));
    CM->operate();
    data = SB->readData();
    ALU.load(I.r1, data);
    PC += 2;
    SR.Z = (data == 0);
    SR.N = Int16(data) < 0;
}

void CentralProcessingUnit::ldwr() {
    Uint16 data;
    AR = ALU.get(I.r2);
    SB->writeAddress(AR);
    SB->writeControl(ControlBus(READ, MEMORY, WORD));
    CM->operate();
    data = SB->readData();
    ALU.load(I.r1, data);
    SR.Z = (data == 0);
    SR.N = Int16(data) < 0;
}

void CentralProcessingUnit::ldba() {
    Uint8 data;
    AR = PC;
    SB->writeAddress(AR);
    SB->writeControl(ControlBus(READ, MEMORY, WORD));
    CM->operate();
    data = SB->readData();
    ALU.load(I.r1, data);
    SR.Z = (data == 0);
    PC++;
}

void CentralProcessingUnit::ldbi() {
    Uint8 data;
    AR = PC;
    SB->writeAddress(AR);
    SB->writeControl(ControlBus(READ, MEMORY, WORD));
    CM->operate();
    SB->writeAddress(SB->readData());
    SB->writeControl(ControlBus(READ, MEMORY, WORD));
    CM->operate();
    data = SB->readData();
    ALU.load(I.r1, data);
    SR.Z = (data == 0);
    PC += 2;
}

void CentralProcessingUnit::ldbr() {
    Uint8 data;
    AR = ALU.get(I.r2);
    SB->writeAddress(AR);
    SB->writeControl(ControlBus(READ, MEMORY, WORD));
    CM->operate();
    data = SB->readData();
    ALU.load(I.r1, data);
    SR.Z = (data == 0);
}

CentralMemory::CentralMemory(SystemBus* pSB, Uint16 psize) :SB(pSB), size(psize) {
    for(Uint16 i = 0; i < size; i++) {
        M.push_back(0x00);
    }
}

void CentralMemory::loadProgram(string path, bool bin, Logger* logger) {
    ifstream file("binaries/" + path);
    if(!file) {
        cout << logger->warning << "File " << path << " does not exist!" << logger->reset << endl;
        return;        
    }
    if(bin) {
        char s[9];
        for(Uint16 i = 0; i < size && !file.eof(); i++) {
            file.getline(s, 9);
            M[i] = math::binstrToUint8(s);
        }
    }
    else {
        char s[3];
        for(Uint16 i = 0; i < size && !file.eof(); i++) {
            file.getline(s, 3);
            M[i] = math::hexstrToUint8(s);
        }
    }
}

void CentralMemory::operate() {
    Uint16 AB = SB->getAddress();
    Uint16 DB = SB->readData();
    ControlBus CB = SB->getControl();
    if(!CB.M) return;
    if(AB >= size) return;
    if(CB.R) {
        if(!CB.W) {
            DB = M[AB];
        }
        else {
            Uint8 a;
            Uint16 b;
            a = M[AB];
            b = M[AB + 1] << 8;
            DB = a | b;
        }
        SB->writeData(DB);
    }
    else {
        if(!CB.W) {
            M[AB] = DB;
        }
        else {
            Uint8 a, b;
            a = DB | 0xFF;
            b = (DB | 0xFF00) >> 8;
            M[AB] = a;
            M[AB + 1] = b;
        }
    }
}

InputOutputDevices::InputOutputDevices(SystemBus* pSB) :SB(pSB) {}