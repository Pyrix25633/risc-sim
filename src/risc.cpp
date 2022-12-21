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

void SystemBus::cleanAddress() {
    AB = 0x0;
}

void SystemBus::cleanData() {
    DB = 0x0;
}

void SystemBus::cleanControl() {
    CB.R = WRITE;
    CB.M = INPUTOUTPUT;
    CB.W = BYTE;
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
    reset();
}

void ArithmeticLogicUnit::reset() {
    for(Uint8 i = 0; i <= 0xF; i++) {
        R[i] = 0x0000;
    }
}

void ArithmeticLogicUnit::add(Uint8 d, Uint8 s) {
    if(d > 0xF) return;
    if(s > 0xF) return;
    Uint32 resc = R[d] + R[s];
    Int16 a = Int16(R[d]), b = Int16(R[s]), c = a + b;
    Int32 res = a + b;
    R[d] += R[s];
    SR->C = (resc > 0xFFFF);
    SR->V = (c != res);
    SR->N = (Int16(R[d]) < 0x0);
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

CentralProcessingUnit::CentralProcessingUnit(SystemBus* pSB, CentralMemory* pCM, InputOutputDevices* pIOD, InterpreterSettings settings)
    :ALU(ArithmeticLogicUnit(&SR)), SB(pSB), CM(pCM), IOD(pIOD), PC(settings.start), phaseNow(0xFF), phaseNext(0x0), instName("-----"),
    SP(settings.ramSize - 2), IR(0x0), AR(0x0), DR(0x0) {}

void CentralProcessingUnit::reset(InterpreterSettings settings) {
    PC = settings.start;
    phaseNow = 0xFF, phaseNext = 0x0;
    instName = "-----";
    SP = settings.ramSize - 2;
    IR = 0x0;
    AR = 0x0;
    DR = 0x0;
    ALU.reset();
    SR.Z = false;
    SR.N = false;
    SR.C = false;
    SR.V = false;
}

void CentralProcessingUnit::fetchInstruction() {
    AR = PC;
    SB->writeAddress(AR);
    SB->writeControl(ControlBus(READ, MEMORY, WORD));
    CM->operate();
    IR = SB->getData();
    PC += 2;
    instName = "-----";
    phaseNow = 0, phaseNext = 1;
}

void CentralProcessingUnit::decodeInstruction() {
    SB->cleanAddress();
    SB->cleanData();
    SB->cleanControl();
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
    phaseNext = (((I.addressing > 0 && (I.addressing != 3 || (I.opcode != 2 && I.opcode != 3)))
        || I.group == 2 || (I.group == 3 && (I.opcode == 0 || I.opcode == 8))) ? 2 : 3);
    instName = decodeInstName();
}

void CentralProcessingUnit::fetchOperand() {
    SB->cleanAddress();
    SB->cleanData();
    SB->cleanControl();
    switch(I.addressing) {
        case 0x0: //16bit after the instruction
            AR = PC;
            SB->writeAddress(AR);
            SB->writeControl(ControlBus(READ, MEMORY, WORD));
            CM->operate();
            break;
        case 0x1: //LD$I
            AR = PC;
            SB->writeAddress(AR);
            SB->writeControl(ControlBus(READ, MEMORY, WORD));
            CM->operate();
            break;
        case 0x2: //$$$A
            if(I.opcode == 0x0 || I.opcode == 0x1) { //LD$A
                AR = PC;
                SB->writeAddress(AR);
                SB->writeControl(ControlBus(READ, MEMORY, WORD));
                CM->operate();
                AR = SB->getData();
                SB->writeAddress(AR);
                SB->writeControl(ControlBus(READ, MEMORY, WORD));
                CM->operate();
            }
            else { //ST$A
                AR = PC;
                SB->writeAddress(AR);
                SB->writeControl(ControlBus(READ, MEMORY, WORD));
                CM->operate();
            }
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
                            cp();
                            break;
                        case 0x8:
                            push();
                            break;
                        case 0x9:
                            pop();
                            break;
                        case 0xD:
                            sprd();
                            break;
                        case 0xE:
                            spwr();
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
                            stwa();
                            break;
                        case 0x3:
                            stba();
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
                            stwr();
                            break;
                        case 0x3:
                            stbr();
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
                case 0x1:
                    inb();
                    break;
                case 0x3:
                    outb();
                    break;
                case 0x4:
                    tsti();
                    break;
                case 0x5:
                    tsto();
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
                    br();
                    break;
                case 0x1:
                    jmp();
                    break;
                case 0x2:
                    jmpz();
                    break;
                case 0x3:
                    jmpnz();
                    break;
                case 0x4:
                    jmpn();
                    break;
                case 0x5:
                    jmpnn();
                    break;
                case 0x6:
                    jmpc();
                    break;
                case 0x7:
                    jmpv();
                    break;
                case 0x8:
                    call();
                    break;
                case 0x9:
                    ret();
                    break;
                case 0xF:
                    hlt();
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
                case 0x1:
                    return "INB";
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
                    return "HLT";
                default:
                    phaseNext = 0xFF;
                    return "ERR!";
            }
    }
    return "ERR!";
}

void CentralProcessingUnit::ldwi() {
    Uint16 data = SB->getData();
    ALU.load(I.ra, data);
    PC += 2;
    SR.Z = (data == 0);
    SR.N = Int16(data) < 0;
}

void CentralProcessingUnit::ldwa() {
    Uint16 data = SB->getData();
    ALU.load(I.ra, data);
    PC += 2;
    SR.Z = (data == 0);
    SR.N = Int16(data) < 0;
}

void CentralProcessingUnit::ldwr() {
    Uint16 data = SB->getData();
    ALU.load(I.ra, data);
    SR.Z = (data == 0);
    SR.N = Int16(data) < 0;
}

void CentralProcessingUnit::ldbi() {
    Uint8 data = SB->getData();
    ALU.load(I.ra, data);
    SR.Z = (data == 0);
    PC++;
}

void CentralProcessingUnit::ldba() {
    Uint8 data = SB->getData();
    ALU.load(I.ra, data);
    SR.Z = (data == 0);
    PC += 2;
}

void CentralProcessingUnit::ldbr() {
    Uint8 data = SB->getData();
    ALU.load(I.ra, data);
    SR.Z = (data == 0);
}

void CentralProcessingUnit::stwa() {
    AR = SB->getData();
    DR = ALU.get(I.ra);
    SB->writeAddress(AR);
    SB->writeData(DR);
    SB->writeControl(ControlBus(WRITE, MEMORY, WORD));
    CM->operate();
    PC += 2;
}

void CentralProcessingUnit::stwr() {
    AR = ALU.get(I.rb);
    DR = ALU.get(I.ra);
    SB->writeAddress(AR);
    SB->writeData(DR);
    SB->writeControl(ControlBus(WRITE, MEMORY, WORD));
    CM->operate();
}

void CentralProcessingUnit::stba() {
    AR = SB->getData();
    DR = ALU.get(I.ra);
    SB->writeAddress(AR);
    SB->writeData(DR);
    SB->writeControl(ControlBus(WRITE, MEMORY, BYTE));
    CM->operate();
    PC += 2;
}

void CentralProcessingUnit::stbr() {
    AR = ALU.get(I.rb);
    DR = ALU.get(I.ra);
    SB->writeAddress(AR);
    SB->writeData(DR);
    SB->writeControl(ControlBus(WRITE, MEMORY, BYTE));
    CM->operate();
}

void CentralProcessingUnit::cp() {
    ALU.load(I.ra, ALU.get(I.rb));
}

void CentralProcessingUnit::push() {
    AR = SP;
    DR = ALU.get(I.ra);
    SB->writeAddress(AR);
    SB->writeData(DR);
    SB->writeControl(ControlBus(WRITE, MEMORY, WORD));
    CM->operate();
    SP -= 2;
}

void CentralProcessingUnit::pop() {
    SP += 2;
    AR = SP;
    SB->writeAddress(AR);
    SB->writeControl(ControlBus(READ, MEMORY, WORD));
    CM->operate();
    ALU.load(I.ra, SB->getData());
}

void CentralProcessingUnit::sprd() {
    ALU.load(I.ra, SP);
}

void CentralProcessingUnit::spwr() {
    SP = ALU.get(I.ra);
}

void CentralProcessingUnit::inb() {
    AR = SB->getData();
    SB->writeAddress(AR);
    SB->writeControl(ControlBus(READ, INPUTOUTPUT, BYTE));
    IOD->operate();
    Uint8 data = SB->getData();
    ALU.load(I.ra, data);
    SR.Z = (data == 0x0);
    PC += 2;
}

void CentralProcessingUnit::outb() {
    AR = SB->getData();
    DR = ALU.get(I.ra);
    SB->writeAddress(AR);
    SB->writeData(DR);
    SB->writeControl(ControlBus(WRITE, INPUTOUTPUT, BYTE));
    IOD->operate();
    PC += 2;
}

void CentralProcessingUnit::tsti() {
    if(SB->getData() == 0x1) {
        SR.Z = IOD->getSent();
    }
    else SR.Z = 0;
    PC += 2;
}

void CentralProcessingUnit::tsto() {
    SR.Z = (SB->getData() == 0x0);
    PC += 2;
}

void CentralProcessingUnit::br() {
    PC = SB->getData();
}

void CentralProcessingUnit::jmp() {
    Uint16 offset;
    if((I.offset >> 7) == 1) offset = 0xFF00 | I.offset;
    else offset = I.offset;
    PC += offset;
}

void CentralProcessingUnit::jmpz() {
    if(SR.Z) jmp();
}

void CentralProcessingUnit::jmpnz() {
    if(!SR.Z) jmp();
}

void CentralProcessingUnit::jmpn() {
    if(SR.N) jmp();
}

void CentralProcessingUnit::jmpnn() {
    if(!SR.N) jmp();
}

void CentralProcessingUnit::jmpc() {
    if(SR.C) jmp();
}

void CentralProcessingUnit::jmpv() {
    if(SR.V) jmp();
}

void CentralProcessingUnit::call() {
    PC += 2;
    Uint16 address = SB->getData();
    AR = SP;
    DR = PC;
    SB->writeAddress(AR);
    SB->writeData(DR);
    SB->writeControl(ControlBus(WRITE, MEMORY, WORD));
    CM->operate();
    SP -= 2;
    PC = address;
}

void CentralProcessingUnit::ret() {
    SP += 2;
    AR = SP;
    SB->writeAddress(AR);
    SB->writeControl(ControlBus(READ, MEMORY, WORD));
    CM->operate();
    PC = SB->getData();
}

void CentralProcessingUnit::hlt() {
    phaseNext = 0xF0;
}

CentralMemory::CentralMemory(SystemBus* pSB, Uint32 psize) :SB(pSB), size(psize) {
    for(Uint32 i = 0; i < size; i++) {
        M.push_back(0x00);
    }
}

void CentralMemory::reset(Uint32 psize) {
    size = psize;
    M.clear();
    for(Uint32 i = 0; i < size; i++) {
        M.push_back(0x00);
    }
}

void CentralMemory::loadProgram(InterpreterSettings* settings, Logger* logger) {
    ifstream file("binaries/" + settings->file);
    if(!file) {
        cout << logger->warning << "File " << settings->file << " does not exist!" << logger->reset << endl;
        return;        
    }
    char s[100];
    switch(settings->type) {
        case 0:
            for(Uint32 i = 0; i < size && !file.eof(); i++) {
                file.getline(s, 100);
                M[i] = math::binstrToUint8(s);
            }
            break;
        case 1:
            for(Uint32 i = 0; i < size && !file.eof(); i++) {
                file.getline(s, 100);
                M[i] = math::hexstrToUint8(s);
            }
            break;
        default:
            try {
                cout << logger->getStringTime() << logger->warning << "Assembling file into hex executable"
                    << logger->reset << endl;
                ofstream outFile("binaries/" + settings->file + ".hex");
                vector<string> asmLines;
                while(!file.eof()) {
                    file.getline(s, 100);
                    string line = s;
                    int commentPos = line.find(';');
                    if(commentPos != string::npos) {
                        line = line.substr(0, commentPos);
                    }
                    Int8 i = 0;
                    while(i < line.length() && line[i] == ' ') {
                        i++;
                    }
                    line = line.substr(i);
                    i = line.length() - 1;
                    while(i >= 0 && line[i] == ' ') {
                        i--;
                    }
                    line = line.substr(0, i + 1);
                    if(line.length() == 0) continue;
                    asmLines.push_back(line);
                }
                vector<string> labels;
                for(string l : asmLines) {
                    int labelPos = l.find(':');
                    if(labelPos != string::npos) labels.push_back(l.substr(0, labelPos));
                }
                vector<string> hexLines;
                for(string l : asmLines) {
                    int labelPos = l.find(':');
                    string label = "";
                    if(labelPos != string::npos) {
                        label = l.substr(0, labelPos + 1);
                        l = l.substr(labelPos + 1);
                        Int8 i = 0;
                        while(i < l.length() && l[i] == ' ') {
                            i++;
                        }
                        l = l.substr(i);
                    }
                    string inst, arg1 = "", arg2 = "";
                    int spacePos = l.find(' ');
                    if(spacePos != string::npos) {
                        inst = l.substr(0, spacePos);
                        l = l.substr(spacePos + 1);
                    }
                    else inst = l;
                    spacePos = l.find(' ');
                    if(spacePos != string::npos) {
                        arg1 = l.substr(0, spacePos);
                        l = l.substr(spacePos);
                    }
                    else arg1 = l;
                    spacePos = l.find(' ');
                    if(spacePos != string::npos) {
                        arg2 = l.substr(spacePos + 1);
                    }
                    if(inst.substr(0, 2) == "LD" && inst.length() == 4) {
                        switch(inst[3]) {
                            case 'I': //LDWI or LDBI
                                hexLines.push_back(label + math::argumentToRegister(arg1) + "0");
                                if(inst[2] == 'W') {
                                    hexLines.push_back("10");
                                    for(string le : math::wordToLittleEndian(arg2))
                                        hexLines.push_back(le);
                                }
                                else {
                                    hexLines.push_back("11");
                                    hexLines.push_back(arg2);
                                }
                                break;
                            case 'A': //LDWA or LDBA
                                hexLines.push_back(label + math::argumentToRegister(arg1) + "0");
                                if(inst[2] == 'W') hexLines.push_back("20");
                                else hexLines.push_back("21");
                                if(!math::vectorContains(labels, arg2))
                                    for(string le : math::wordToLittleEndian(arg2))
                                        hexLines.push_back(le);
                                else
                                    hexLines.push_back(arg2); hexLines.push_back("");
                                break;
                            case 'R': //LDWR or LDBR
                                hexLines.push_back(label + math::argumentToRegister(arg1) + math::argumentToRegister(arg2));
                                if(inst[2] == 'W') hexLines.push_back("30");
                                else hexLines.push_back("31");
                        }
                    }
                    else if(inst.substr(0, 2) == "ST" && inst.length() == 4) {
                        switch(inst[3]) {
                            case 'A': //STWA or STBA
                                hexLines.push_back(label + math::argumentToRegister(arg1) + "0");
                                if(inst[2] == 'W') hexLines.push_back("22");
                                else hexLines.push_back("23");
                                if(!math::vectorContains(labels, arg2))
                                    for(string le : math::wordToLittleEndian(arg2))
                                        hexLines.push_back(le);
                                else
                                    hexLines.push_back(arg2); hexLines.push_back("");
                                break;
                            case 'R': //STWR or STBR
                                hexLines.push_back(label + math::argumentToRegister(arg1) + math::argumentToRegister(arg2));
                                if(inst[2] == 'W') hexLines.push_back("32");
                                else hexLines.push_back("33");
                        }
                    }
                    else if(inst == "CP" || inst == "MV") { //CP
                        hexLines.push_back(label + math::argumentToRegister(arg1) + math::argumentToRegister(arg2));
                        hexLines.push_back("04");
                    }
                    else if(inst == "PUSH") { //PUSH
                        hexLines.push_back(label + math::argumentToRegister(arg1) + "0");
                        hexLines.push_back("08");
                    }
                    else if(inst == "POP") { //POP
                        hexLines.push_back(label + math::argumentToRegister(arg1) + "0");
                        hexLines.push_back("09");
                    }
                    else if(inst == "SPWR") { //SPWR
                        hexLines.push_back(label + math::argumentToRegister(arg1) + "0");
                        hexLines.push_back("0D");
                    }
                    else if(inst == "SPRD") { //SPRD
                        hexLines.push_back(label + math::argumentToRegister(arg1) + "0");
                        hexLines.push_back("0E");
                    }
                    else if(inst == "ADD") { //ADD
                        hexLines.push_back(label + math::argumentToRegister(arg1) + math::argumentToRegister(arg2));
                        hexLines.push_back("40");
                    }
                    else if(inst == "SUB") { //SUB
                        hexLines.push_back(label + math::argumentToRegister(arg1) + math::argumentToRegister(arg2));
                        hexLines.push_back("41");
                    }
                    else if(inst == "NOT") { //NOT
                        hexLines.push_back(label + math::argumentToRegister(arg1) + "0");
                        hexLines.push_back("42");
                    }
                    else if(inst == "AND") { //AND
                        hexLines.push_back(label + math::argumentToRegister(arg1) + math::argumentToRegister(arg2));
                        hexLines.push_back("43");
                    }
                    else if(inst == "OR") { //OR
                        hexLines.push_back(label + math::argumentToRegister(arg1) + math::argumentToRegister(arg2));
                        hexLines.push_back("44");
                    }
                    else if(inst == "XOR") { //XOR
                        hexLines.push_back(label + math::argumentToRegister(arg1) + math::argumentToRegister(arg2));
                        hexLines.push_back("45");
                    }
                    else if(inst == "INC") { //INC
                        hexLines.push_back(label + math::argumentToRegister(arg1) + "0");
                        hexLines.push_back("48");
                    }
                    else if(inst == "DEC") { //DEC
                        hexLines.push_back(label + math::argumentToRegister(arg1) + "0");
                        hexLines.push_back("49");
                    }
                    else if(inst == "LSH") { //LSH
                        hexLines.push_back(label + math::argumentToRegister(arg1) + "0");
                        hexLines.push_back("4A");
                    }
                    else if(inst == "RSH") { //RSH
                        hexLines.push_back(label + math::argumentToRegister(arg1) + "0");
                        hexLines.push_back("4B");
                    }
                    else if(inst == "INB") { //INB
                        hexLines.push_back(label + math::argumentToRegister(arg1) + "0");
                        hexLines.push_back("81");
                        for(string le : math::wordToLittleEndian(arg2))
                            hexLines.push_back(le);
                    }
                    else if(inst == "OUTB") { //OUTB
                        hexLines.push_back(label + math::argumentToRegister(arg1) + "0");
                        hexLines.push_back("83");
                        for(string le : math::wordToLittleEndian(arg2))
                            hexLines.push_back(le);
                    }
                    else if(inst.substr(0, 3) == "TST" && inst.length() == 4) { //TSTI
                        hexLines.push_back(label + "00");
                        if(inst[3] == 'I') hexLines.push_back("84");
                        else hexLines.push_back("85");
                        for(string le : math::wordToLittleEndian(arg1))
                            hexLines.push_back(le);
                    }
                    else if(inst == "BR") { //BR
                        hexLines.push_back(label + "00");
                        hexLines.push_back("C0");
                        if(!math::vectorContains(labels, arg1))
                            for(string le : math::wordToLittleEndian(arg1))
                                hexLines.push_back(le);
                        else
                            hexLines.push_back(arg1); hexLines.push_back("");
                    }
                    else if(inst.substr(0, 3) == "JMP" && (inst.length() >= 3 && inst.length() <= 5)) {
                        hexLines.push_back(label + arg1);
                        if(inst.length() == 3) //JMP
                            hexLines.push_back("C1");
                        else if(inst.substr(3) == "Z") //JMPZ
                            hexLines.push_back("C2");
                        else if(inst.substr(3) == "NZ") //JMPNZ
                            hexLines.push_back("C3");
                        else if(inst.substr(3) == "N") //JMPN
                            hexLines.push_back("C4");
                        else if(inst.substr(3) == "NZ") //JMPNN
                            hexLines.push_back("C5");
                        else if(inst.substr(3) == "C") //JMPC
                            hexLines.push_back("C6");
                        else if(inst.substr(3) == "V") //JMPC
                            hexLines.push_back("C7");
                    }
                    else if(inst == "CALL") { //CALL
                        hexLines.push_back(label + "00");
                        hexLines.push_back("C8");
                        if(!math::vectorContains(labels, arg1))
                            for(string le : math::wordToLittleEndian(arg1))
                                hexLines.push_back(le);
                        else
                            hexLines.push_back(arg1); hexLines.push_back("");
                    }
                    else if(inst == "RET") { //RET
                        hexLines.push_back(label + "00");
                        hexLines.push_back("C9");
                    }
                    else if(inst == "HLT") { //HLT
                        hexLines.push_back(label + "00");
                        hexLines.push_back("CF");
                    }
                    else if(inst == "WORD") { //WORD
                        vector<string> le = math::wordToLittleEndian(arg1);
                            hexLines.push_back(label + le[0]);
                            hexLines.push_back(le[1]);
                    }
                    else if(inst == "BYTE") { //BYTE
                        hexLines.push_back(label + arg1);
                    }
                }
                struct Label {
                    string name;
                    Uint16 address;
                };
                vector<Label> labelAddressAssociations;
                for(int i = 0; i < hexLines.size(); i++) {
                    string line = hexLines[i];
                    int labelPos = line.find(':');
                    if(labelPos != string::npos) {
                        Label l;
                        l.name = line.substr(0, labelPos);
                        l.address = i;
                        labelAddressAssociations.push_back(l);
                        hexLines[i] = line.substr(labelPos + 1);
                    }
                }
                for(int i = 0; i < hexLines.size(); i++) {
                    string line = hexLines[i];
                    int labelPos;
                    for(Label l : labelAddressAssociations) {
                        labelPos = line.find(l.name);
                        if(labelPos != string::npos) {
                            if(hexLines[i + 1] != "") {
                                hexLines[i] = math::Uint8ToHexstr(Uint8(l.address - i - 2));
                            }
                            else {
                                int j = 0;
                                for(string s : math::wordToLittleEndian(math::Uint16ToHexstr(l.address))) {
                                    hexLines[i + j] = s;
                                    j++;
                                }
                            }
                            break;
                        }
                    }
                }
                for(string l : hexLines) {
                    outFile << l << endl;
                }
                outFile.close();
                cout << logger->getStringTime() << logger->success << "Succesfully assembled file into hex executable"
                    << logger->reset << endl;
                settings->file = settings->file + ".hex";
                settings->type = 1;
                settings->ramSize = hexLines.size() + 0xF0;
                for(Label l : labelAddressAssociations) {
                    if(l.name == "START") {
                        settings->start = l.address;
                    }
                }
                loadProgram(settings, logger);
            }
            catch(int e) {
                cout << logger->getStringTime() << logger->error << "Error while assembling file into hex executable"
                    << logger->reset << endl;
            }
    }
    file.close();
}

void CentralMemory::operate() {
    Uint16 AB = SB->getAddress();
    Uint16 DB = SB->getData();
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
            a = DB & 0xFF;
            b = (DB & 0xFF00) >> 8;
            M[AB] = a;
            M[AB + 1] = b;
        }
    }
}

Uint8 CentralMemory::get(Uint16 address) {
    if(address >= size) return 0x0;
    return M[address];
}

InputOutputDevices::InputOutputDevices(SystemBus* pSB) :SB(pSB) {
    sent = false;
}

void InputOutputDevices::reset() {
    line0 = line1 = line2 = line3 = "";
}

void InputOutputDevices::input(Uint8 i) {
    key = i;
}

void InputOutputDevices::operate() {
    ControlBus control = SB->getControl();
    Uint16 address = SB->getAddress();
    Uint16 data = SB->getData();
    if(control.M) return;
    Uint8 l0size = line0.size(), l1size = line1.size(), l2size = line2.size(), l3size = line3.size();
    if(address == 0x0 && !control.R) { //Output monitor
        Uint8 data = SB->getData();
        string s = "!";
        s[0] = data;
        if(data == '\n') {
            line0 = line1;
            line1 = line2;
            line2 = line3;
            line3 = "";
        }
        else if(data == '\r') {
            string spaces = "                    ";
            if(l0size == 20) {
                if(l1size == 20) {
                    if(l2size == 20) {
                        if(l3size == 20) {
                            line0 = line1;
                            line1 = line2;
                            line2 = line3;
                            line3 = spaces;
                        }
                        else {
                            line3 += spaces.substr(0, 20 - l3size);
                        }
                    }
                    else {
                        line2 += spaces.substr(0, 20 - l2size);
                    }
                }
                else {
                    line1 += spaces.substr(0, 20 - l1size);
                }
            }
            else {
                line0 += spaces.substr(0, 20 - l0size);
            }
        }
        else if(l0size == 20) {
            if(l1size == 20) {
                if(l2size == 20) {
                    if(l3size == 20) {
                        line0 = line1;
                        line1 = line2;
                        line2 = line3;
                        line3 = s;
                    }
                    else {
                        line3 += s;
                    }
                }
                else {
                    line2 += s;
                }
            }
            else {
                line1 += s;
            }
        }
        else {
            line0 += s;
        }
    }
    else if(address == 0x1 && control.R) { //Input keyboard
        SB->writeData(key);
        sent = (key != 0x0);
        key = 0x0;
    }
}

void InputOutputDevices::getLines(string &l0, string &l1, string &l2, string &l3) {
    l0 = line0;
    l1 = line1;
    l2 = line2;
    l3 = line3;
}

bool InputOutputDevices::getSent() {
    if(sent) {
        sent = false;
        return true;
    }
    else return false;
}