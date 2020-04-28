//
// Created by cuent on 23/04/2020.
//

#include "../MIX/MIXInstruction.h"

MIXInstruction::MIXInstruction(string n, function<void(MIXWord&, int, int)> func) {
    name = n;
    executionProc = func;
}

void MIXInstruction::execute(MIXWord address, int index, int field) {
    executionProc(address, index, field);
}

void MIXInstruction::execute(MIXWord address, int index, int left, int right) {
    executionProc(address, index, left * 8 + right);
}
