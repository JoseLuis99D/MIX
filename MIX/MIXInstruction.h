#ifndef MIX_MIXINSTRUCTION_H
#define MIX_MIXINSTRUCTION_H

#include <functional>
#include "MIXWord.h"

struct InstructionInfo{
    string name;
    int default_field;
    int opcode;
    int time;
    bool operator==(InstructionInfo inf){
        return (name==inf.name) && (default_field==inf.default_field)
            && (opcode==inf.opcode) && (time==inf.time);
    }
};

class MIXInstruction {
    function<void(MIXWord&, int, int)> executionProc;
public:
    string name;
    MIXInstruction(string, function<void(MIXWord&, int, int)>);
    void execute(MIXWord, int, int);
    void execute(MIXWord, int, int, int);
};

#endif //MIX_MIXINSTRUCTION_H
