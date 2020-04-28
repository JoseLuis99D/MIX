//
// Created by cuent on 23/04/2020.
//

#ifndef MIX_MIXMACHINE_H
#define MIX_MIXMACHINE_H

#include <vector>
#include <map>
#include "MIXWord.h"
#include "MIXInstruction.h"

enum Comparison{
    LESS,
    GREATER,
    EQUAL
};

enum Devices{
    TAPE1 = 0,
    TAPE2,
    TAPE3,
    TAPE4,
    TAPE5,
    TAPE6,
    TAPE7,
    TAPE8,
    DISK1,
    DISK2,
    DISK3,
    DISK4,
    DISK5,
    DISK6,
    DISK7,
    DISK8,
    CARD_READER,
    CARD_PUNCH,
    LINE_PRINTER,
    TERMINAL,
    PAPER_TAPE
};

class MIXMachine {
    bool running;
    MIXWord *memory;
    MIXWord A;
    MIXWord X;
    MIXWord J;
    MIXWord *I;
    bool overflow;
    Comparison CI;
    vector<int> breakpoints;
    int execution_time;
    int PC;
    vector<MIXInstruction> instructionTable;
    //MIXDevice[] devices;

    void InitMachine();
    static void makeCharTable();
    static void makeInstructionList();
    void makeInstructionTable();

public:
    static vector<InstructionInfo> INSTRUCTION_LIST;
    static map<char, int> CHAR_TABLE;

    MIXMachine();
    static void prepareMIXMachine();
    void loadImage(vector<MemoryCell>);
    void loadDeck();
    InstructionInfo disassemble(MIXWord);
    void run();
    void addBreakpoint(int location);
    void clearBreakpoint(int which);
    void clearAllBreakpoints();
    int getBreakpoint(int i);
    int breakpointCount();
    void step();
};


#endif //MIX_MIXMACHINE_H
