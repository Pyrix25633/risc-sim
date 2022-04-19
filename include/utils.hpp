#pragma once

#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/value.h>
#include <time.h>

#include "math.hpp"

#define READ true
#define WRITE false
#define MEMORY true
#define INPUTOUTPUT false
#define WORD true
#define BYTE false

using namespace std;
using namespace Json;

typedef int8_t Int8;
typedef int16_t Int16;
typedef int32_t Int32;

struct ControlBus;
struct Instruction;
struct StatusRegister;
struct Logger;
struct RendererFlags;
struct InterpreterSettings;
struct ConsoleSettings;
struct WindowSettings;

/**
 * @brief Structure that contains the control bus variables
 * @param R Read
 * @param M Memory
 * @param W Word
*/
struct ControlBus {
    bool R, M, W;
    /**
     * @brief Constructor
    */
    ControlBus();
    /**
     * @brief Constructor
    */
    ControlBus(bool r, bool m, bool w);
    /**
     * @brief Function to reset control bus
    */
    void reset();
};

/**
 * @brief Structure that contains instruction data
 * @param group Instruction group
 * @param addressing Addressing method
 * @param opcode Operation code
 * @param r1 First register
 * @param r2 Second register
 * @param offset Offset for jmp instructions
*/
struct Instruction {
    Uint8 group;
    Uint8 addressing;
    Uint8 opcode;
    Uint8 ra, rb;
    Uint8 offset;
    /**
     * @brief Constructor
    */
    Instruction();
};

/**
 * @brief Structure that contains the status register variables
 * @param Z Zero
 * @param C Carry
 * @param N Negative
 * @param V Overflow
*/
struct StatusRegister {
    /**
     * @brief Constructor
    */
    StatusRegister();
    bool Z, N, C, V;
};

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

class SystemBus;
class ArithmeticLogicUnit;
class CentralProcessingUnit;
class CentralMemory;
class InputOutputDevices;

class SystemBus {
    public:
        /**
         * @brief Constructor
        */
        SystemBus();
        /**
         * @brief Function to write on address bus
         * @param a Address to write
        */
        void writeAddress(Uint16 a);
        /**
         * @brief Function to write on data bus
         * @param d Data to write
        */
        void writeData(Uint16 d);
        /**
         * @brief Function to write on control bus
         * @param c Control to write
        */
        void writeControl(ControlBus c);
        /**
         * @brief Function to read from address bus and empty it
         * @return What is written on address bus
        */
        Uint16 readAddress();
        /**
         * @brief Function to read from data bus and empty it
         * @return What is written on data bus
        */
        Uint16 readData();
        /**
         * @brief Function to read from control bus and empty it
         * @return What is written on control bus
        */
        ControlBus readControl();
        /**
         * @brief Function to get the content of the address bus
         * @return What is written on address bus
        */
        Uint16 getAddress();
        /**
         * @brief Function to get the content of the data bus
         * @return What is written on data bus
        */
        Uint16 getData();
        /**
         * @brief Function to get the content of the control bus
         * @return What is written on control bus
        */
        ControlBus getControl();
        /**
         * @brief Function to reset the system bus
        */
        void resetBus();
    private:
        Uint16 AB; //Address Bus
        Uint16 DB; //Data Bus
        ControlBus CB; //Control Bus
};

class ArithmeticLogicUnit {
    public:
        /**
         * @brief Constructor
        */
        ArithmeticLogicUnit(StatusRegister* pSR);
        /**
         * @brief Function to add two numbers, d += s
         * @param d Destination register
         * @param s Source register
        */
        void add(Uint8 d, Uint8 s);
        /**
         * @brief Function to subtruct two numbers, d -= s
         * @param d Destination register
         * @param s Source register
        */
        void sub(Uint8 d, Uint8 s);
        /**
         * @brief Function to do bitwise not, r = ~r
         * @param r Register
        */
        void bNot(Uint8 r);
        /**
         * @brief Function to do bitwise and, d = d & s
         * @param d Destination register
         * @param s Source register
        */
        void bAnd(Uint8 d, Uint8 s);
        /**
         * @brief Function to do bitwise or, d = d | s
         * @param d Destination register
         * @param s Source register
        */
        void bOr(Uint8 r, Uint8 s);
        /**
         * @brief Function to do bitwise xor, d = d ^ s
         * @param d Destination register
         * @param s Source register
        */
        void bXor(Uint8 d, Uint8 s);
        /**
         * @brief Function to increment, r++
         * @param r Register
        */
        void inc(Uint8 r);
        /**
         * @brief Function to decrement, r--
         * @param r Register
        */
        void dec(Uint8 r);
        /**
         * @brief Function to left shift, r << 1
         * @param r Register
        */
        void lShift(Uint8 r);
        /**
         * @brief Function to right shift, r >> 1
         * @param r Register
        */
        void rShift(Uint8 r);
        /**
         * @brief Function to load a word in a register
         * @param r Register number
         * @param word The word to load
        */
        void load(Uint8 r, Uint16 word);
        /**
         * @brief Funtion to get the word from a register
         * @param r The register number
        */
        Uint16 get(Uint8 r);
    private:
        Uint16 R[16]; //Registers from 0 to 15
        StatusRegister* SR; //Status register pointer
};

class CentralProcessingUnit{
    public:
        /**
         * @brief Constructor
         * @param pSB System bus pointer
         * @param pCM Central memory pointer
         * @param pIOD Input/output devices pointer
         * @param start Address of first program code line
        */
        CentralProcessingUnit(SystemBus* pSB, CentralMemory* pCM, InputOutputDevices* pIOD, Uint16 start);
        /**
         * @brief Function that fetches the instruction from the memory
        */
        void fetchInstruction();
        /**
         * @brief Function to decodes the instruction
        */
        void decodeInstruction();
        /**
         * @brief Function to fetch the operand
        */
        void fetchOperand();
        void executeInstruction();
        /**
         * @brief Function to get the Program Counter value
         * @returns The program counter value
        */
        Uint16 getPC();
        /**
         * @brief Function to get the Instruction Register value
         * @returns The instruction register value
        */
        Uint16 getIR();
        /**
         * @brief Function to get the Status Register value
         * @returns The status register value
        */
        StatusRegister getSR();
        /**
         * @brief Function to get the Address Register value
         * @returns The adress register value
        */
        Uint16 getAR();
        /**
         * @brief Function to get the Data Register value
         * @returns The data register value
        */
        Uint16 getDR();
        /**
         * @brief Function to get the Stack Pointer value
         * @returns The stack pointer value
        */
        Uint16 getSP();
        /**
         * @brief Function to get the instruction name
         * @return The instriction name, type string
        */
        string getInstName();
    private:
        Uint16 PC; //Program Counter
        Uint16 SP; //Stack Pointer
        Uint16 IR; //Instruction Register
        Uint16 AR; //Address Register
        Uint16 DR; //Data Register
        StatusRegister SR; //Status Register
        Instruction I; //Decoded Instruction
        ArithmeticLogicUnit ALU; //Arithmetic Logic Unit
        SystemBus* SB; //System Bus pointer
        CentralMemory* CM; //Central Memory pointer
        InputOutputDevices* IOD; //Input Output Devices pointer
        Uint8 phaseNow, phaseNext; //Phases 0:IF 1:ID 2:OF 3:IE
        string instName; //Instruction name, for GUI
        /**
         * @brief Function to decode the instruction name
         * @returns The instruction name
        */
        string decodeInstName();
        /**
         * @brief Function that loads a word, that is after the instruction, in a register
        */
        void ldwi();
        /**
         * @brief Function that loads a word, from an address that is after the instruction, in a register
        */
        void ldwa();
        /**
         * @brief Function that loads a word, from an address that is in a register, in a register
        */
        void ldwr();
        /**
         * @brief Function that loads a byte, that is after the instruction, in a register
        */
        void ldbi();
        /**
         * @brief Function that loads a byte, from an address that is after the instruction, in a register
        */
        void ldba();
        /**
         * @brief Function that loads a byte, from an address that is in a register, in a register
        */
        void ldbr();
};

class CentralMemory {
    public:
        /**
         * @brief Contructor
         * @param pSB System bus pointer
         * @param psize Memory fixed size
        */
        CentralMemory(SystemBus* pSB, Uint16 psize);
        /**
         * @brief Program to load the program
        */
        void loadProgram(string path, bool bin, Logger* logger);
        /**
         * @brief Function that reads the system bus and operate
        */
        void operate();
    private:
        vector<Uint8> M; //All memory bytes
        Uint16 size; //Memory size
        SystemBus* SB; //System Bus pointer
};

class InputOutputDevices {
    public:
        /**
         * @brief Constructor
         * @param pSB System bus pointer
        */
        InputOutputDevices(SystemBus* pSB);
    private:
        SystemBus* SB; //System Bus pointer
};