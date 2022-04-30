#pragma once

#include "utils.hpp"
struct Logger;
struct InterpreterSettings;

using namespace std;

#define READ true
#define WRITE false
#define MEMORY true
#define INPUTOUTPUT false
#define WORD true
#define BYTE false

struct ControlBus;
struct Instruction;
struct StatusRegister;

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
         * @brief Function to clean the address bus
        */
        void cleanAddress();
        /**
         * @brief Function to clean the data bus
        */
        void cleanData();
        /**
         * @brief Function to clean the control bus
        */
        void cleanControl();
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
         * @brief Function to reset the ALU
        */
        void reset();
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
         * @param settings The interpreter settings
        */
        CentralProcessingUnit(SystemBus* pSB, CentralMemory* pCM, InputOutputDevices* pIOD, InterpreterSettings settings);
        /**
         * @brief Function to reset the CPU
         * @param settings The interpreter settings
        */
        void reset(InterpreterSettings settings);
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
        /**
         * @brief Function to execute the instruction 
        */
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
        /**
         * @brief Function to get a register from the ALU that is private
         * @param r The registrer number
         * @returns The value, type Uint16
        */
        Uint16 getR(Uint8 r);
        /**
         * @brief Function to get the two phases
         * @param now Variable in which store phase now
         * @param next Variable in which store phase next
        */
        void getPhases(Uint8 &now, Uint8 &next);
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
        /**
         * @brief Function that stores a word, from an address that is after the instruction
        */
        void stwa();
        /**
         * @brief Function that stores a word, from an address that is in a register
        */
        void stwr();
        /**
         * @brief Function that stores a byte, from an address that is after the instruction
        */
        void stba();
        /**
         * @brief Function that stores a byte, from an address that is in a register
        */
        void stbr();
        /**
         * @brief function that copies the value of a register in another register
        */
        void cp();
        /**
         * @brief function that saves a register in the stack
        */
        void push();
        /**
         * @brief function that restores a register from the stack
        */
        void pop();
        /**
         * @brief function that read the stack pointer and put its value on a register
        */
        void sprd();
        /**
         * @brief Function that write the stack pointer with the value of a register
        */
        void spwr();
        /**
         * @brief Function to modify the program counter with an address after the instruction
        */
        void br();
        /**
         * @brief Function to modify the program counter with an offset 
        */
        void jmp();
        /**
         * @brief Function to jump if zero
        */
        void jmpz();
        /**
         * @brief Function to jump if not zero
        */
        void jmpnz();
        /**
         * @brief Function to jump if negative
        */
        void jmpn();
        /**
         * @brief Function to jump if not negative
        */
        void jmpnn();
        /**
         * @brief Function to jump if carry
        */
        void jmpc();
        /**
         * @brief Function to jump if overflow
        */
        void jmpv();
        /**
         * @brief Function to call a subroutine
        */
        void call();
        /**
         * @brief Function to return from a subroutine
        */
        void ret();
        /**
         * @brief Function to exit the program
        */
        void hlt();
};

class CentralMemory {
    public:
        /**
         * @brief Contructor
         * @param pSB System bus pointer
         * @param psize Memory fixed size
        */
        CentralMemory(SystemBus* pSB, Uint32 psize);
        /**
         * @brief Function to reset the CM
         * @param psize Memory fixed size
        */
        void reset(Uint32 psize);
        /**
         * @brief Program to load the program
        */
        void loadProgram(string path, bool bin, Logger* logger);
        /**
         * @brief Function that reads the system bus and operate
        */
        void operate();
        /**
         * @brief Function to get the value of a cell
         * @param address The cell address, type Uint16
         * @returns The cell value, type Uint8
        */
        Uint8 get(Uint16 address);
    private:
        vector<Uint8> M; //All memory bytes
        Uint32 size; //Memory size
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