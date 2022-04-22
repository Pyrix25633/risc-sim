#include "risc.hpp"

using namespace std;

ControlBus::ControlBus() {
    this->reset();
}

ControlBus::ControlBus(bool r, bool m, bool w) :R(r), M(m), W(w) {}

void ControlBus::reset() {
    R = WRITE;
    M = INPUTOUTPUT;
    W = BYTE;
}

Instruction::Instruction() :group(0), addressing(0), opcode(0), ra(0), rb(0), offset(0) {}

StatusRegister::StatusRegister() :Z(false), C(false), N(false), V(false) {}

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
    for(Uint8 i = 0; i <= 0xF; i++) {
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
    Int16 a = Int16(R[d]), b = Int16(R[s]);
    Int32 res = a - b, resc = R[d] + math::twosComplement(R[s]);
    R[d] -= R[s];
    SR->C = (resc > 0xFFFF);
    SR->V = (Int16(R[d]) != res);
    SR->N = (res < 0x0);
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
    :ALU(ArithmeticLogicUnit(&SR)), SB(pSB), CM(pCM), IOD(pIOD), PC(start), phaseNow(0xFF), phaseNext(0x0), instName("-----"),
    SP(0x0), IR(0x0), AR(0x0), DR(0x0) {}

void CentralProcessingUnit::fetchInstruction() {
    AR = PC;
    SB->writeAddress(AR);
    SB->writeControl(ControlBus(READ, MEMORY, WORD));
    CM->operate();
    IR = SB->readData();
    PC += 2;
    instName = "-----";
    phaseNow = 0, phaseNext = 1;
}

void CentralProcessingUnit::decodeInstruction() {
    I.group = (IR & 0xC000) >> 14;
    I.addressing = (IR & 0x3000) >> 12;
    I.opcode = (IR & 0x0F00) >> 8;
    if(I.group == 0x3) {
        I.offset = (IR & 0x00FF);
    }
    else {
        I.ra = (IR & 0x00F0) >> 4;
        I.rb = (IR & 0x000F);
    }
    phaseNow = 1;
    phaseNext = ((I.addressing > 0) ? 2 : 3);
    instName = decodeInstName();
}

void CentralProcessingUnit::fetchOperand() {
    switch(I.addressing) {
        case 0x1: //LD$I
            AR = PC;
            SB->writeAddress(AR);
            SB->writeControl(ControlBus(READ, MEMORY, WORD));
            CM->operate();
            break;
        case 0x2: //LD$A
            AR = PC;
            SB->writeAddress(AR);
            SB->writeControl(ControlBus(READ, MEMORY, WORD));
            CM->operate();
            SB->writeAddress(SB->readData());
            SB->writeControl(ControlBus(READ, MEMORY, WORD));
            CM->operate();
            break;
        case 0x3: //LD$R
            AR = ALU.get(I.rb);
            SB->writeAddress(AR);
            SB->writeControl(ControlBus(READ, MEMORY, WORD));
            CM->operate();
    }
    phaseNow = 2;
    phaseNext = 3;
}

void CentralProcessingUnit::executeInstruction() {
    phaseNow = 3;
    phaseNext = 0;
    switch(I.group) {
        case 0x0: //Data transfer group
            switch(I.addressing) {
                case 0x0:
                    switch(I.opcode) {
                        case 0x4:
                            break;
                        case 0x8:
                            break;
                        case 0x9:
                            break;
                        case 0xD:
                            break;
                        case 0xE:
                            break;
                        default:
                            phaseNext = 0xFF;
                    }
                    break;
                case 0x1:
                    switch(I.opcode) {
                        case 0x0:
                            ldwi();
                            break;
                        case 0x1:
                            ldbi();
                            break;
                        default:
                            phaseNext = 0xFF;
                    }
                    break;
                case 0x2:
                    switch(I.opcode) {
                        case 0x0:
                            ldwa();
                            break;
                        case 0x1:
                            ldba();
                            break;
                        case 0x2:
                            break;
                        case 0x3:
                            break;
                        default:
                            phaseNext = 0xFF;
                    }
                    break;
                case 0x3:
                    switch(I.opcode) {
                        case 0x0:
                            ldwr();
                            break;
                        case 0x1:
                            ldbr();
                            break;
                        case 0x2:
                            break;
                        case 0x3:
                            break;
                        default:
                            phaseNext = 0xFF;
                    }
            }
            break;
        case 0x1: //Arithmetic-logic group
            if(I.addressing != 0x0) {
                phaseNext = 0xFF;
            }
            switch(I.opcode) {
                case 0x0:
                    ALU.add(I.rb, I.ra);
                    break;
                case 0x1:
                    ALU.sub(I.rb, I.ra);
                    break;
                case 0x2:
                    ALU.bNot(I.ra);
                    break;
                case 0x3:
                    ALU.bAnd(I.rb, I.ra);
                    break;
                case 0x4:
                    ALU.bOr(I.rb, I.ra);
                    break;
                case 0x5:
                    ALU.bXor(I.rb, I.ra);
                    break;
                case 0x8:
                    ALU.inc(I.ra);
                    break;
                case 0x9:
                    ALU.dec(I.ra);
                    break;
                case 0xA:
                    ALU.lShift(I.ra);
                    break;
                case 0xB:
                    ALU.rShift(I.ra);
                    break;
                default:
                    phaseNext = 0xFF;
            }
            break;
        case 0x2: //Input/output group
            if(I.addressing != 0x0) {
                phaseNext = 0xFF;
            }
            switch(I.opcode) {
                case 0x0:
                    break;
                case 0x1:
                    break;
                case 0x2:
                    break;
                case 0x3:
                    break;
                case 0x4:
                    break;
                case 0x5:
                    break;
                default:
                    phaseNext = 0xFF;
            }
            break;
        case 0x3: //Control group
            if(I.addressing != 0x0) {
                phaseNext = 0xFF;
            }
            switch (I.opcode) {
                case 0x0:
                    break;
                case 0x1:
                    break;
                case 0x2:
                    break;
                case 0x3:
                    break;
                case 0x4:
                    break;
                case 0x5:
                    break;
                case 0x6:
                    break;
                case 0x7:
                    break;
                case 0x8:
                    break;
                case 0x9:
                    break;
                case 0xF:
                    break;
                default:
                    phaseNext = 0xFF;
            }
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

string CentralProcessingUnit::getInstName() {
    return instName;
}

Uint16 CentralProcessingUnit::getR(Uint8 r) {
    return ALU.get(r);
}

void CentralProcessingUnit::getPhases(Uint8 &now, Uint8 &next) {
    now = phaseNow;
    next = phaseNext;
    return;
}

string CentralProcessingUnit::decodeInstName() {
    switch(I.group) {
        case 0x0: //Data transfer group
            switch(I.addressing) {
                case 0x0:
                    switch(I.opcode) {
                        case 0x4:
                            return "MV";
                        case 0x8:
                            return "PUSH";
                        case 0x9:
                            return "POP";
                        case 0xD:
                            return "SPRD";
                        case 0xE:
                            return "SPWR";
                        default:
                            phaseNext = 0xFF;
                            return "ERR!";
                    }
                    break;
                case 0x1:
                    switch(I.opcode) {
                        case 0x0:
                            return "LDWI";
                        case 0x1:
                            return "LDBI";
                        default:
                            phaseNext = 0xFF;
                            return "ERR!";
                    }
                    break;
                case 0x2:
                    switch(I.opcode) {
                        case 0x0:
                            return "LDWA";
                        case 0x1:
                            return "LDBA";
                        case 0x2:
                            return "STWA";
                        case 0x3:
                            return "STBA";
                        default:
                            phaseNext = 0xFF;
                            return "ERR!";
                    }
                    break;
                case 0x3:
                    switch(I.opcode) {
                        case 0x0:
                            return "LDWR";
                        case 0x1:
                            return "LDBR";
                        case 0x2:
                            return "STWR";
                        case 0x3:
                            return "STBR";
                        default:
                            phaseNext = 0xFF;
                            return "ERR!";
                    }
            }
            break;
        case 0x1: //Arithmetic-logic group
            if(I.addressing != 0x0) {
                phaseNext = 0xFF;
                return "ERR!";
            }
            switch(I.opcode) {
                case 0x0:
                    return "ADD";
                case 0x1:
                    return "SUB";
                case 0x2:
                    return "NOT";
                case 0x3:
                    return "AND";
                case 0x4:
                    return "OR";
                case 0x5:
                    return "XOR";
                case 0x8:
                    return "INC";
                case 0x9:
                    return "DEC";
                case 0xA:
                    return "LSH";
                case 0xB:
                    return "RSH";
                default:
                    phaseNext = 0xFF;
                    return "ERR!";
            }
            break;
        case 0x2: //Input/output group
            if(I.addressing != 0x0) {
                phaseNext = 0xFF;
                return "ERR!";
            }
            switch(I.opcode) {
                case 0x0:
                    return "INW";
                case 0x1:
                    return "INB";
                case 0x2:
                    return "OUTW";
                case 0x3:
                    return "OUTB";
                case 0x4:
                    return "TSTI";
                case 0x5:
                    return "TSTO";
                default:
                    phaseNext = 0xFF;
                    return "ERR!";
            }
            break;
        case 0x3: //Control group
            if(I.addressing != 0x0) {
                phaseNext = 0xFF;
                return "ERR!";
            }
            switch (I.opcode) {
                case 0x0:
                    return "BR";
                case 0x1:
                    return "JMP";
                case 0x2:
                    return "JMPZ";
                case 0x3:
                    return "JMPNZ";
                case 0x4:
                    return "JMPN";
                case 0x5:
                    return "JMPNN";
                case 0x6:
                    return "JMPC";
                case 0x7:
                    return "JMPV";
                case 0x8:
                    return "CALL";
                case 0x9:
                    return "RET";
                case 0xF:
                    return "HALT";
                default:
                    phaseNext = 0xFF;
                    return "ERR!";
            }
    }
    return "ERR!";
}

void CentralProcessingUnit::ldwi() {
    Uint16 data = SB->readData();
    ALU.load(I.ra, data);
    PC += 2;
    SR.Z = (data == 0);
    SR.N = Int16(data) < 0;
}

void CentralProcessingUnit::ldwa() {
    Uint16 data = SB->readData();
    ALU.load(I.ra, data);
    PC += 2;
    SR.Z = (data == 0);
    SR.N = Int16(data) < 0;
}

void CentralProcessingUnit::ldwr() {
    Uint16 data = SB->readData();
    ALU.load(I.ra, data);
    SR.Z = (data == 0);
    SR.N = Int16(data) < 0;
}

void CentralProcessingUnit::ldbi() {
    Uint8 data = SB->readData();
    ALU.load(I.ra, data);
    SR.Z = (data == 0);
    PC++;
}

void CentralProcessingUnit::ldba() {
    Uint8 data = SB->readData();
    ALU.load(I.ra, data);
    SR.Z = (data == 0);
    PC += 2;
}

void CentralProcessingUnit::ldbr() {
    Uint8 data = SB->readData();
    ALU.load(I.ra, data);
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
        char s[100];
        for(Uint16 i = 0; i < size && !file.eof(); i++) {
            file.getline(s, 100);
            M[i] = math::binstrToUint8(s);
        }
    }
    else {
        char s[100];
        for(Uint16 i = 0; i < size && !file.eof(); i++) {
            file.getline(s, 100);
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