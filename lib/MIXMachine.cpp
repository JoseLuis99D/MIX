#include <iostream>
#include <cstdio>
#include <algorithm>
#include <bitset>
#include "../MIX/MIXMachine.h"


MIXMachine::MIXMachine() {
    InitMachine();
}

void MIXMachine::InitMachine() {
    memory = new MIXWord[4000];
    for(int i=0; i<4000; i++)
        memory[i] = MIXWord();
    PC = 0;
    A = MIXWord();
    X = MIXWord();
    J = MIXWord();
    I = new MIXWord[6];

    for(int i = 0; i < 6; i++)
        I[i] = MIXWord();

    overflow = false;
    CI = EQUAL;
    running = false;
    execution_time = 0;
}

void MIXMachine::loadImage(vector<MemoryCell> image) {
    for(auto cell : image)
        memory[cell.location] = cell.contents;
}

void MIXMachine::loadDeck() {
    MIXInstruction in_instr = *find_if(instructionTable.begin(), instructionTable.end(),
            [](MIXInstruction instr){ return instr.name == "IN"; });
    MIXWord word(0);
    in_instr.execute(word, 0, CARD_READER);
    PC = 0;
    J.setValue(0);
    run();
}

InstructionInfo MIXMachine::disassemble(MIXWord word) {
    InstructionInfo first = *find_if(INSTRUCTION_LIST.begin(), INSTRUCTION_LIST.end(),
            [&word](InstructionInfo inf){ return inf.opcode == word.getField(5); });
    InstructionInfo last = *find_if(INSTRUCTION_LIST.rbegin(), INSTRUCTION_LIST.rend(),
            [&word](InstructionInfo inf){ return inf.opcode == word.getField(5); });
    if (first == last){
        return first;
    }else{
        InstructionInfo instr = *find_if(INSTRUCTION_LIST.rbegin(), INSTRUCTION_LIST.rend(),
                [&word](InstructionInfo inf){ return inf.opcode == word.getField(5)
                    && inf.default_field == word.getField(4); });
        return instr;
    }
}

void MIXMachine::run() {
    if (!running)
        running = true;
    execution_time = 0;
    while(running && PC < 4000)
        step();
}

void MIXMachine::addBreakpoint(int location) {
    breakpoints.push_back(location);
}

void MIXMachine::clearBreakpoint(int n) {
    breakpoints.erase(breakpoints.begin() + n);
}

void MIXMachine::clearAllBreakpoints() {
    breakpoints.clear();
}

int MIXMachine::getBreakpoint(int i) {
    return breakpoints.at(i);
}

int MIXMachine::breakpointCount() {
    return breakpoints.size();
}

void MIXMachine::step() {
    MIXWord w = memory[PC];

    InstructionInfo info = disassemble(w);
    if (&info != NULL){
        InstructionInfo ii = info;
        vector<MIXInstruction> ilist;
        copy_if(instructionTable.begin(), instructionTable.end(), ilist.begin(),
                [&ii](MIXInstruction i){ return i.name == ii.name; });
        if (ilist.size() == 0)
            cerr << "Instruction not found: '" << ii.name << "'" << endl;
        else{
            MIXInstruction instr = ilist.at(0);
            MIXWord word(w.getField(0, 2));
            instr.execute(word, w.getField(3), w.getField(4));
            execution_time += ii.time;
        }
    }
    if(find(breakpoints.begin(), breakpoints.end(), PC) != breakpoints.end())
        running = false;
}

void MIXMachine::makeInstructionTable() {
    /* Load operators */
    MIXInstruction LDA("LDA", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        A.setValue(memory[m].getField(field/8, field%8));
        PC++;
    });
    MIXInstruction LDX("LDX", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));;
        X.setValue(memory[m].getField(field/8, field%8));
        PC++;
    });
    MIXInstruction LD1("LD1", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));;
        I[0].setValue(memory[m].getField(field/8, field%8));
        PC++;
    });
    MIXInstruction LD2("LD2", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));;
        I[1].setValue(memory[m].getField(field/8, field%8));
        PC++;
    });
    MIXInstruction LD3("LD3", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));;
        I[2].setValue(memory[m].getField(field/8, field%8));
        PC++;
    });
    MIXInstruction LD4("LD4", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));;
        I[3].setValue(memory[m].getField(field/8, field%8));
        PC++;
    });
    MIXInstruction LD5("LD5", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));;
        I[4].setValue(memory[m].getField(field/8, field%8));
        PC++;
    });
    MIXInstruction LD6("LD6", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));;
        I[5].setValue(memory[m].getField(field/8, field%8));
        PC++;
    });
    MIXInstruction LDAN("LDAN", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        A.setValue(-memory[m].getField(field/8, field%8));
        PC++;
    });
    MIXInstruction LDXN("LDXN", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));;
        X.setValue(-memory[m].getField(field/8, field%8));
        PC++;
    });
    MIXInstruction LD1N("LD1N", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));;
        I[0].setValue(-memory[m].getField(field/8, field%8));
        PC++;
    });
    MIXInstruction LD2N("LD2N", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));;
        I[1].setValue(-memory[m].getField(field/8, field%8));
        PC++;
    });
    MIXInstruction LD3N("LD3N", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));;
        I[2].setValue(-memory[m].getField(field/8, field%8));
        PC++;
    });
    MIXInstruction LD4N("LD4N", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));;
        I[3].setValue(-memory[m].getField(field/8, field%8));
        PC++;
    });
    MIXInstruction LD5N("LD5N", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));;
        I[4].setValue(-memory[m].getField(field/8, field%8));
        PC++;
    });
    MIXInstruction LD6N("LD6N", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));;
        I[5].setValue(-memory[m].getField(field/8, field%8));
        PC++;
    });

    vector<MIXInstruction> load_instructions = {LDA, LDX, LD1, LD2, LD3, LD4, LD5, LD6,
        LDA, LDX, LD1, LD2, LD3, LD4, LD5, LD6};
    instructionTable.insert(instructionTable.end(), load_instructions.begin(),
            load_instructions.end());

    /* Store operations */
    MIXInstruction STA("STA", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index - 1].getValue() : 0));
        if (field > 0){
            int l = field/8;
            int r = field%8;
            bool useSign = false;
            if (l == 0){
                l++;
                useSign = true;
            }
            int width = r - l;
            int v = A.getField(5-width, 5);

            memory[m].setField(l, r, v);
            if(useSign)
                memory[m].setSign(A.getSign());
        }
        else
            memory[m].setSign(A.getSign());
        PC++;
    });
    MIXInstruction STX("STX", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index - 1].getValue() : 0));
        if (field > 0){
            int l = field/8;
            int r = field%8;
            bool useSign = false;
            if (l == 0){
                l++;
                useSign = true;
            }
            int width = r - l;
            int v = X.getField(5-width, 5);

            memory[m].setField(l, r, v);
            if(useSign)
                memory[m].setSign(X.getSign());
        }
        else
            memory[m].setSign(X.getSign());
        PC++;
    });
    MIXInstruction ST1("ST1", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index - 1].getValue() : 0));
        if (field > 0){
            int l = field/8;
            int r = field%8;
            bool useSign = false;
            if (l == 0){
                l++;
                useSign = true;
            }
            int width = r - l;
            int v = I[0].getField(5-width, 5);

            memory[m].setField(l, r, v);
            if(useSign)
                memory[m].setSign(I[0].getSign());
        }
        else
            memory[m].setSign(I[0].getSign());
        PC++;
    });
    MIXInstruction ST2("ST2", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index - 1].getValue() : 0));
        if (field > 0){
            int l = field/8;
            int r = field%8;
            bool useSign = false;
            if (l == 0){
                l++;
                useSign = true;
            }
            int width = r - l;
            int v = I[1].getField(5-width, 5);

            memory[m].setField(l, r, v);
            if(useSign)
                memory[m].setSign(I[1].getSign());
        }
        else
            memory[m].setSign(I[1].getSign());
        PC++;
    });
    MIXInstruction ST3("ST3", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index - 1].getValue() : 0));
        if (field > 0){
            int l = field/8;
            int r = field%8;
            bool useSign = false;
            if (l == 0){
                l++;
                useSign = true;
            }
            int width = r - l;
            int v = I[2].getField(5-width, 5);

            memory[m].setField(l, r, v);
            if(useSign)
                memory[m].setSign(I[2].getSign());
        }
        else
            memory[m].setSign(I[2].getSign());
        PC++;
    });
    MIXInstruction ST4("ST4", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index - 1].getValue() : 0));
        if (field > 0){
            int l = field/8;
            int r = field%8;
            bool useSign = false;
            if (l == 0){
                l++;
                useSign = true;
            }
            int width = r - l;
            int v = I[3].getField(5-width, 5);

            memory[m].setField(l, r, v);
            if(useSign)
                memory[m].setSign(I[3].getSign());
        }
        else
            memory[m].setSign(I[3].getSign());
        PC++;
    });
    MIXInstruction ST5("ST5", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index - 1].getValue() : 0));
        if (field > 0){
            int l = field/8;
            int r = field%8;
            bool useSign = false;
            if (l == 0){
                l++;
                useSign = true;
            }
            int width = r - l;
            int v = I[4].getField(5-width, 5);

            memory[m].setField(l, r, v);
            if(useSign)
                memory[m].setSign(I[4].getSign());
        }
        else
            memory[m].setSign(I[4].getSign());
        PC++;
    });
    MIXInstruction ST6("ST6", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index - 1].getValue() : 0));
        if (field > 0){
            int l = field/8;
            int r = field%8;
            bool useSign = false;
            if (l == 0){
                l++;
                useSign = true;
            }
            int width = r - l;
            int v = I[5].getField(5-width, 5);

            memory[m].setField(l, r, v);
            if(useSign)
                memory[m].setSign(I[5].getSign());
        }
        else
            memory[m].setSign(I[5].getSign());
        PC++;
    });
    MIXInstruction STJ("STJ", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index - 1].getValue() : 0));
        int l = field/8;
        int r = field%8;
        int width = r - l;
        int v = abs(J.getField(5-width, 5));
        memory[m].setField(l, r, v);
        PC++;
    });
    MIXInstruction STZ("STZ", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index - 1].getValue() : 0));
        int l = field/8;
        int r = field%8;
        memory[m].setField(l, r, 0);
        PC++;
    });

    vector<MIXInstruction> store_instructions = {STA, STX, ST1, ST2, ST3, ST4,
        ST5, ST6, STJ, STZ};
    instructionTable.insert(instructionTable.end(), store_instructions.begin(),
                            store_instructions.end());

    /* Arithmetic operations */
    MIXInstruction ADD("ADD", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index - 1].getValue() : 0));
        int l = field/8;
        int r = field%8;
        int v = memory[m].getField(l, r);
        int sum = A.getValue() + v;

        if (abs(sum) > MIXWord::MaxValue()){
            overflow = true;
            int result = abs(sum) & MIXWord::MaxValue();
            if(sum < 0)
                A.setSign(Negative);
            else if(sum > 0)
                A.setSign(Positive);
            A.setUnsignedValue(result);
        }
        else
            A.setValue(sum);
        PC++;
    });
    MIXInstruction SUB("SUB", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index - 1].getValue() : 0));
        int l = field/8;
        int r = field%8;
        int v = memory[m].getField(l, r);
        int sub = A.getValue() - v;

        if (abs(sub) > MIXWord::MaxValue()){
            overflow = true;
            int result = abs(sub) & MIXWord::MaxValue();
            if(sub < 0)
                A.setSign(Negative);
            else if(sub > 0)
                A.setSign(Positive);
            A.setUnsignedValue(result);
        }
        else
            A.setValue(sub);
        PC++;
    });
    MIXInstruction MUL("MUL", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index - 1].getValue() : 0));
        int l = field/8;
        int r = field%8;
        int v = memory[m].getField(l, r);
        long result = (long) v * (long) A.getValue();

        Sign resSign = result < 0 ? Negative : Positive;
        string resultStr = bitset<60>(abs(result)).to_string();

        A.setValue(stoi(resultStr.substr(0, 30), nullptr, 2));
        A.setSign(resSign);
        X.setValue(stoi(resultStr.substr(30), nullptr, 2));
        X.setSign(resSign);
        PC++;
    });
    MIXInstruction DIV("DIV", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index - 1].getValue() : 0));
        int l = field/8;
        int r = field%8;
        int v = memory[m].getField(l, r);
        long result = (long) v * (long) A.getValue();

        if(v == 0 || abs(A.getValue()) >= abs(v))
            overflow = true;
        else{
            int sign = A.getField(0);
            string numerString = bitset<30>(A.getUnsignedValue()).to_string()
                    + bitset<30>(X.getUnsignedValue()).to_string();
            long numer = stol(numerString, nullptr, 2);
            A.setValue(numer/v);
            X.setValue(numer%v);
            X.setField(0, sign);
        }
        PC++;
    });

    vector<MIXInstruction> arithmetic_instructions = {ADD, SUB, MUL, DIV};
    instructionTable.insert(instructionTable.end(), arithmetic_instructions.begin(),
                            arithmetic_instructions.end());

    /* Address transfer operations */
    MIXInstruction ENTA("ENTA", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        A.setValue(m);
        if (word.getValue() == 0)
            A.setSign(word.getSign());
        PC++;
    });
    MIXInstruction ENTX("ENTX", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        X.setValue(m);
        if (word.getValue() == 0)
            X.setSign(word.getSign());
        PC++;
    });
    MIXInstruction ENT1("ENT1", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[0].setValue(m);
        if (word.getValue() == 0)
            I[0].setSign(word.getSign());
        PC++;
    });
    MIXInstruction ENT2("ENT2", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[1].setValue(m);
        if (word.getValue() == 0)
            I[1].setSign(word.getSign());
        PC++;
    });
    MIXInstruction ENT3("ENT3", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[2].setValue(m);
        if (word.getValue() == 0)
            I[2].setSign(word.getSign());
        PC++;
    });
    MIXInstruction ENT4("ENT4", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[3].setValue(m);
        if (word.getValue() == 0)
            I[3].setSign(word.getSign());
        PC++;
    });
    MIXInstruction ENT5("ENT5", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[4].setValue(m);
        if (word.getValue() == 0)
            I[4].setSign(word.getSign());
        PC++;
    });
    MIXInstruction ENT6("ENT6", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[5].setValue(m);
        if (word.getValue() == 0)
            I[5].setSign(word.getSign());
        PC++;
    });
    MIXInstruction ENNA("ENNA", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        A.setValue(-m);
        if (word.getValue() == 0)
            A.setSign(word.getSign());
        PC++;
    });
    MIXInstruction ENNX("ENNX", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        X.setValue(-m);
        if (word.getValue() == 0)
            X.setSign(word.getSign());
        PC++;
    });
    MIXInstruction ENN1("ENN1", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[0].setValue(-m);
        if (word.getValue() == 0)
            I[0].setSign(word.getSign());
        PC++;
    });
    MIXInstruction ENN2("ENN2", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[1].setValue(-m);
        if (word.getValue() == 0)
            I[1].setSign(word.getSign());
        PC++;
    });
    MIXInstruction ENN3("ENN3", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[2].setValue(-m);
        if (word.getValue() == 0)
            I[2].setSign(word.getSign());
        PC++;
    });
    MIXInstruction ENN4("ENN4", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[3].setValue(-m);
        if (word.getValue() == 0)
            I[3].setSign(word.getSign());
        PC++;
    });
    MIXInstruction ENN5("ENN5", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[4].setValue(-m);
        if (word.getValue() == 0)
            I[4].setSign(word.getSign());
        PC++;
    });
    MIXInstruction ENN6("ENN6", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[5].setValue(-m);
        if (word.getValue() == 0)
            I[5].setSign(word.getSign());
        PC++;
    });
    MIXInstruction INCA("INCA", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        int value = A.getValue() + m;
        if(abs(value) > MIXWord::MaxValue()){
            overflow = true;
            int result = abs(value) & MIXWord::MaxValue();
            if(value < 0)
                A.setSign(Negative);
            else
                A.setSign(Positive);
            A.setUnsignedValue(result);
        }
        else
            A.setValue(value);
        PC++;
    });
    MIXInstruction INCX("INCX", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        int value = X.getValue() + m;
        if(abs(value) > MIXWord::MaxValue()){
            overflow = true;
            int result = abs(value) & MIXWord::MaxValue();
            if(value < 0)
                X.setSign(Negative);
            else
                X.setSign(Positive);
            X.setUnsignedValue(result);
        }
        else
            X.setValue(value);
        PC++;
    });
    MIXInstruction INC1("INC1", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[0].setValue(I[0].getValue() + m);
        PC++;
    });
    MIXInstruction INC2("INC2", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[1].setValue(I[1].getValue() + m);
        PC++;
    });
    MIXInstruction INC3("INC3", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[2].setValue(I[2].getValue() + m);
        PC++;
    });
    MIXInstruction INC4("INC4", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[3].setValue(I[3].getValue() + m);
        PC++;
    });
    MIXInstruction INC5("INC5", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[4].setValue(I[4].getValue() + m);
        PC++;
    });
    MIXInstruction INC6("INC6", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[5].setValue(I[5].getValue() + m);
        PC++;
    });
    MIXInstruction DECA("DECA", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        int value = A.getValue() - m;
        if(abs(value) > MIXWord::MaxValue()){
            overflow = true;
            int result = abs(value) & MIXWord::MaxValue();
            if(value < 0)
                A.setSign(Negative);
            else
                A.setSign(Positive);
            A.setUnsignedValue(result);
        }
        else
            A.setValue(value);
        PC++;
    });
    MIXInstruction DECX("DECX", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        int value = X.getValue() - m;
        if(abs(value) > MIXWord::MaxValue()){
            overflow = true;
            int result = abs(value) & MIXWord::MaxValue();
            if(value < 0)
                X.setSign(Negative);
            else
                X.setSign(Positive);
            X.setUnsignedValue(result);
        }
        else
            X.setValue(value);
        PC++;
    });
    MIXInstruction DEC1("DEC1", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[0].setValue(I[0].getValue() - m);
        PC++;
    });
    MIXInstruction DEC2("DEC2", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[1].setValue(I[1].getValue() - m);
        PC++;
    });
    MIXInstruction DEC3("DEC3", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[2].setValue(I[2].getValue() - m);
        PC++;
    });
    MIXInstruction DEC4("DEC4", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[3].setValue(I[3].getValue() - m);
        PC++;
    });
    MIXInstruction DEC5("DEC5", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[4].setValue(I[4].getValue() - m);
        PC++;
    });
    MIXInstruction DEC6("DEC6", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        I[5].setValue(I[5].getValue() - m);
        PC++;
    });

    vector<MIXInstruction> transfer_instructions = {ENTA, ENTX, ENT1, ENT2, ENT3, ENT4, ENT5, ENT6,
        ENNA, ENNX, ENN1, ENN2, ENN3, ENN4, ENN5, ENN6, INCA, INCX, INC1, INC2, INC3, INC4, INC5,
        INC6, DECA, DECX, DEC1, DEC2, DEC3, DEC4, DEC5, DEC6};
    instructionTable.insert(instructionTable.end(), transfer_instructions.begin(),
                            transfer_instructions.end());

    /* Comparisons operators */
    MIXInstruction CMPA("CMPA", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        int a_val = A.getField(field/8, field%8);
        int m_val = memory[m].getField(field/8, field%8);
        if (a_val > m_val)
            CI = GREATER;
        else if (a_val < m_val)
            CI = LESS;
        else
            CI = EQUAL;
        PC++;
    });
    MIXInstruction CMPX("CMPX", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        int a_val = X.getField(field/8, field%8);
        int m_val = memory[m].getField(field/8, field%8);
        if (a_val > m_val)
            CI = GREATER;
        else if (a_val < m_val)
            CI = LESS;
        else
            CI = EQUAL;
        PC++;
    });
    MIXInstruction CMP1("CMP1", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        MIXWord j(I[0].getField(4, 5));
        int a_val = j.getField(field/8, field%8);
        int m_val = memory[m].getField(field/8, field%8);
        if (a_val > m_val)
            CI = GREATER;
        else if (a_val < m_val)
            CI = LESS;
        else
            CI = EQUAL;
        PC++;
    });
    MIXInstruction CMP2("CMP2", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        MIXWord j(I[1].getField(4, 5));
        int a_val = j.getField(field/8, field%8);
        int m_val = memory[m].getField(field/8, field%8);
        if (a_val > m_val)
            CI = GREATER;
        else if (a_val < m_val)
            CI = LESS;
        else
            CI = EQUAL;
        PC++;
    });
    MIXInstruction CMP3("CMP3", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        MIXWord j(I[2].getField(4, 5));
        int a_val = j.getField(field/8, field%8);
        int m_val = memory[m].getField(field/8, field%8);
        if (a_val > m_val)
            CI = GREATER;
        else if (a_val < m_val)
            CI = LESS;
        else
            CI = EQUAL;
        PC++;
    });
    MIXInstruction CMP4("CMP4", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        MIXWord j(I[3].getField(4, 5));
        int a_val = j.getField(field/8, field%8);
        int m_val = memory[m].getField(field/8, field%8);
        if (a_val > m_val)
            CI = GREATER;
        else if (a_val < m_val)
            CI = LESS;
        else
            CI = EQUAL;
        PC++;
    });
    MIXInstruction CMP5("CMP5", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        MIXWord j(I[4].getField(4, 5));
        int a_val = j.getField(field/8, field%8);
        int m_val = memory[m].getField(field/8, field%8);
        if (a_val > m_val)
            CI = GREATER;
        else if (a_val < m_val)
            CI = LESS;
        else
            CI = EQUAL;
        PC++;
    });
    MIXInstruction CMP6("CMP6", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        MIXWord j(I[5].getField(4, 5));
        int a_val = j.getField(field/8, field%8);
        int m_val = memory[m].getField(field/8, field%8);
        if (a_val > m_val)
            CI = GREATER;
        else if (a_val < m_val)
            CI = LESS;
        else
            CI = EQUAL;
        PC++;
    });

    vector<MIXInstruction> comparisons_instructions = {CMPA, CMPX, CMP1, CMP2, CMP3, CMP4,
        CMP5, CMP6};
    instructionTable.insert(instructionTable.end(), comparisons_instructions.begin(),
                            comparisons_instructions.end());

    /* Jumps operations */
    MIXInstruction JMP("JMP", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        J.setValue(PC + 1);
        PC = m;
    });
    MIXInstruction JSJ("JSJ", [this](MIXWord& word, int index, int field){
        PC = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
    });
    MIXInstruction JOV("JOV", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(overflow){
            J.setValue(PC + 1);
            PC = m;
        }else
            PC++;
    });
    MIXInstruction JNOV("JNOV", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if (!overflow){
            J.setValue(PC + 1);
            PC = m;
        }else{
            overflow = false;
            PC++;
        }
    });
    MIXInstruction JL("JL", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if (CI == LESS){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction JG("JG", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if (CI == GREATER){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction JE("JE", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if (CI == EQUAL){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction JGE("JGE", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if (CI != LESS){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction JNE("JNE", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if (CI != EQUAL){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction JLE("JLE", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if (CI != GREATER){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction JAN("JAN", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(A.getValue() < 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction JAZ("JAZ", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(A.getValue() == 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction JAP("JAP", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(A.getValue() > 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction JANN("JANN", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(A.getValue() >= 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction JANZ("JANZ", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(A.getValue() != 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction JANP("JANP", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(A.getValue() <= 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction JXN("JXN", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(X.getValue() < 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction JXZ("JXZ", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(X.getValue() == 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction JXP("JXP", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(X.getValue() > 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction JXNN("JXNN", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(X.getValue() >= 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction JXNZ("JXNZ", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(X.getValue() != 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction JXNP("JXNP", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(X.getValue() <= 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J1N("J1N", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[0].getValue() < 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J1Z("J1Z", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[0].getValue() == 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J1P("J1P", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[0].getValue() > 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J1NN("J1NN", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[0].getValue() >= 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J1NZ("J1NZ", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[0].getValue() != 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J1NP("J1NP", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[0].getValue() <= 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J2N("J2N", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[1].getValue() < 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J2Z("J2Z", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[1].getValue() == 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J2P("J2P", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[1].getValue() > 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J2NN("J2NN", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[1].getValue() >= 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J2NZ("J2NZ", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[1].getValue() != 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J2NP("J2NP", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[1].getValue() <= 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J3N("J3N", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[2].getValue() < 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J3Z("J3Z", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[2].getValue() == 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J3P("J3P", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[2].getValue() > 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J3NN("J3NN", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[2].getValue() >= 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J3NZ("J3NZ", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[2].getValue() != 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J3NP("J3NP", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[2].getValue() <= 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J4N("J4N", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[3].getValue() < 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J4Z("J4Z", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[3].getValue() == 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J4P("J4P", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[3].getValue() > 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J4NN("J4NN", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[3].getValue() >= 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J4NZ("J4NZ", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[3].getValue() != 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J4NP("J4NP", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[3].getValue() <= 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J5N("J5N", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[4].getValue() < 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J5Z("J5Z", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[4].getValue() == 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J5P("J5P", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[4].getValue() > 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J5NN("J5NN", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[4].getValue() >= 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J5NZ("J5NZ", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[4].getValue() != 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J5NP("J5NP", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[4].getValue() <= 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J6N("J6N", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[5].getValue() < 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J6Z("J6Z", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[5].getValue() == 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J6P("J6P", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[5].getValue() > 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J6NN("J6NN", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[5].getValue() >= 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J6NZ("J6NZ", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[5].getValue() != 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });
    MIXInstruction J6NP("J6NP", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        if(I[5].getValue() <= 0){
            J.setValue(PC + 1);
            PC = m;
        }
        else
            PC++;
    });

    vector<MIXInstruction> jumps_instructions = {JMP, JSJ, JOV, JNOV, JL, JG, JE,
        JGE, JNE, JLE, JAN, JAZ, JAP, JANN, JANZ, JANP, JXN, JXZ, JXP, JXNN, JXNZ, JXNP,
        J1N, J1Z, J1P, J1NN, J1NZ, J1NP, J2N, J2Z, J2P, J2NN, J2NZ, J2NP,
        J3N, J3Z, J3P, J3NN, J3NZ, J3NP, JAN, J4Z, J4P, J4NN, J4NZ, J4NP,
        J5N, J5Z, J5P, J5NN, J5NZ, J5NP, J5N, J5Z, J5P, J5NN, J5NZ, J5NP};
    instructionTable.insert(instructionTable.end(), jumps_instructions.begin(),
                            jumps_instructions.end());

    /* Miscellaneous operations */
    MIXInstruction SLA("SLA", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        string result = bitset<30>(A.getUnsignedValue()).to_string();
        for (int k = 0; k < m; k++)
            result += "000000";
        result = result.substr(result.length() - 30);
        A.setUnsignedValue(stoi(result, nullptr, 2));
        PC++;
    });
    MIXInstruction SRA("SRA", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        string result = bitset<30>(A.getUnsignedValue()).to_string();
        for (int k = 0; k < m; k++)
            result = "000000" + result;
        result = result.substr(0, 30);
        A.setUnsignedValue(stoi(result, nullptr, 2));
        PC++;
    });
    MIXInstruction SLAX("SLAX", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        string result = bitset<30>(A.getUnsignedValue()).to_string() + bitset<30>(X.getUnsignedValue()).to_string();
        for (int k = 0; k < m; k++)
            result += "000000";
        result = result.substr(result.length() - 60);
        A.setUnsignedValue(stoi(result.substr(0, 30), nullptr, 2));
        X.setUnsignedValue(stoi(result.substr(30), nullptr, 2));
        PC++;
    });
    MIXInstruction SRAX("SRAX", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        string result = bitset<30>(A.getUnsignedValue()).to_string() + bitset<30>(X.getUnsignedValue()).to_string();
        for (int k = 0; k < m; k++)
            result = "000000" + result;
        result = result.substr(0, 60);
        A.setUnsignedValue(stoi(result.substr(0, 30), nullptr, 2));
        X.setUnsignedValue(stoi(result.substr(30), nullptr, 2));
        PC++;
    });
    MIXInstruction SLC("SLC", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        string result = bitset<30>(A.getUnsignedValue()).to_string() + bitset<30>(X.getUnsignedValue()).to_string();
        result = result.substr(m*6) + result.substr(0, m*6);
        A.setUnsignedValue(stoi(result.substr(0, 30), nullptr, 2));
        X.setUnsignedValue(stoi(result.substr(30), nullptr, 2));
        PC++;
    });
    MIXInstruction SRC("SRC", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        string result = bitset<30>(A.getUnsignedValue()).to_string() + bitset<30>(X.getUnsignedValue()).to_string();
        result = result.substr(60 - m*6) + result.substr(0, 60 - m*6);
        A.setUnsignedValue(stoi(result.substr(0, 30), nullptr, 2));
        X.setUnsignedValue(stoi(result.substr(30), nullptr, 2));
        PC++;
    });
    MIXInstruction MOVE("MOVE", [this](MIXWord& word, int index, int field){
        int m = (int) (word + (index > 0 ? I[index-1].getValue() : 0));
        int target = I[0].getValue();
        for (int j=0; j<field; j++, I[0].setValue(I[0].getValue() + 1))
            memory[target + j].setValue(memory[m + j].getValue());
        PC++;
    });
    MIXInstruction NOP("NOP", [this](MIXWord& word, int index, int field){
        PC++;
    });
    MIXInstruction HLT("HLT", [this](MIXWord& word, int index, int field){
        PC++;
        running = false;
    });

    vector<MIXInstruction> miscellaneous_instructions = {SLA, SRA, SLAX, SRAX, SLC, SRC,
        MOVE, NOP, HLT};
    instructionTable.insert(instructionTable.end(), miscellaneous_instructions.begin(),
                            miscellaneous_instructions.end());

    /* Input/Output operations */
    MIXInstruction IN("IN", [this](MIXWord& word, int index, int field){
        PC++;
    });
    MIXInstruction OUT("OUT", [this](MIXWord& word, int index, int field){
        PC++;
    });
    MIXInstruction IOC("IOC", [this](MIXWord& word, int index, int field){
        PC++;
    });
    MIXInstruction JRED("JRED", [this](MIXWord& word, int index, int field){
        PC++;
    });
    MIXInstruction JBUS("JBUS", [this](MIXWord& word, int index, int field){
        PC++;
    });

    vector<MIXInstruction> io_instructions = {IN, OUT, IOC, JRED, JBUS};
    instructionTable.insert(instructionTable.end(), io_instructions.begin(),
                            io_instructions.end());

    /* Conversion operations */
    MIXInstruction NUM("NUM", [this](MIXWord& word, int index, int field){
        string result = "";
        result += A.getField(1) % 10;
        result += A.getField(2) % 10;
        result += A.getField(3) % 10;
        result += A.getField(4) % 10;
        result += A.getField(5) % 10;
        result += X.getField(1) % 10;
        result += X.getField(2) % 10;
        result += X.getField(3) % 10;
        result += X.getField(4) % 10;
        result += X.getField(5) % 10;
        long lVal = stol(result);
        if (lVal > (long) MIXWord::MaxValue)
            lVal %= 7776;
        A.setUnsignedValue((int) lVal);
        PC++;
    });
    MIXInstruction CHAR("CHAR", [this](MIXWord& word, int index, int field){
        char buffer[10];
        sprintf(buffer, "%010d", A.getUnsignedValue());
        string result(buffer);
        for (int j=0; j<5; j++){
            int b = MIXMachine::CHAR_TABLE[result[j]];
            A.setField(j + 1, b);
        }
        for (int i=5; i<10; i++){
            int b = MIXMachine::CHAR_TABLE[result[i]];
            X.setField(i - 4, b);
        }
        PC++;
    });
}

void MIXMachine::makeCharTable() {
    CHAR_TABLE.insert(pair<char,int>(' ', 0));
    CHAR_TABLE.insert(pair<char,int>('A', 1));
    CHAR_TABLE.insert(pair<char,int>('B', 2));
    CHAR_TABLE.insert(pair<char,int>('C', 3));
    CHAR_TABLE.insert(pair<char,int>('D', 4));
    CHAR_TABLE.insert(pair<char,int>('E', 5));
    CHAR_TABLE.insert(pair<char,int>('F', 6));
    CHAR_TABLE.insert(pair<char,int>('G', 7));
    CHAR_TABLE.insert(pair<char,int>('H', 8));
    CHAR_TABLE.insert(pair<char,int>('I', 9));
    CHAR_TABLE.insert(pair<char,int>('+', 10));
    CHAR_TABLE.insert(pair<char,int>('J', 11));
    CHAR_TABLE.insert(pair<char,int>('K', 12));
    CHAR_TABLE.insert(pair<char,int>('L', 13));
    CHAR_TABLE.insert(pair<char,int>('M', 14));
    CHAR_TABLE.insert(pair<char,int>('N', 15));
    CHAR_TABLE.insert(pair<char,int>('O', 16));
    CHAR_TABLE.insert(pair<char,int>('P', 17));
    CHAR_TABLE.insert(pair<char,int>('Q', 18));
    CHAR_TABLE.insert(pair<char,int>('R', 19));
    CHAR_TABLE.insert(pair<char,int>('+', 20));
    CHAR_TABLE.insert(pair<char,int>('+', 21));
    CHAR_TABLE.insert(pair<char,int>('S', 22));
    CHAR_TABLE.insert(pair<char,int>('T', 23));
    CHAR_TABLE.insert(pair<char,int>('U', 24));
    CHAR_TABLE.insert(pair<char,int>('V', 25));
    CHAR_TABLE.insert(pair<char,int>('W', 26));
    CHAR_TABLE.insert(pair<char,int>('X', 27));
    CHAR_TABLE.insert(pair<char,int>('Y', 28));
    CHAR_TABLE.insert(pair<char,int>('Z', 29));

    CHAR_TABLE.insert(pair<char,int>('0', 30));
    CHAR_TABLE.insert(pair<char,int>('1', 31));
    CHAR_TABLE.insert(pair<char,int>('2', 32));
    CHAR_TABLE.insert(pair<char,int>('3', 33));
    CHAR_TABLE.insert(pair<char,int>('4', 34));
    CHAR_TABLE.insert(pair<char,int>('5', 35));
    CHAR_TABLE.insert(pair<char,int>('6', 36));
    CHAR_TABLE.insert(pair<char,int>('7', 37));
    CHAR_TABLE.insert(pair<char,int>('8', 38));
    CHAR_TABLE.insert(pair<char,int>('9', 39));

    CHAR_TABLE.insert(pair<char,int>('.', 40));
    CHAR_TABLE.insert(pair<char,int>(',', 41));
    CHAR_TABLE.insert(pair<char,int>('(', 42));
    CHAR_TABLE.insert(pair<char,int>(')', 43));
    CHAR_TABLE.insert(pair<char,int>('+', 44));
    CHAR_TABLE.insert(pair<char,int>('-', 45));
    CHAR_TABLE.insert(pair<char,int>('*', 46));
    CHAR_TABLE.insert(pair<char,int>('/', 47));
    CHAR_TABLE.insert(pair<char,int>('=', 48));
    CHAR_TABLE.insert(pair<char,int>('$', 49));
    CHAR_TABLE.insert(pair<char,int>('<', 50));
    CHAR_TABLE.insert(pair<char,int>('>', 51));
    CHAR_TABLE.insert(pair<char,int>('@', 52));
    CHAR_TABLE.insert(pair<char,int>(';', 53));
    CHAR_TABLE.insert(pair<char,int>(':', 54));
    CHAR_TABLE.insert(pair<char,int>('\'', 55));
}

void MIXMachine::makeInstructionList() {
    INSTRUCTION_LIST.push_back(InstructionInfo{"NOP", 0, 0, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ADD", 5, 1, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"SUB", 5, 2, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"MUL", 5, 3, 10 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"DIV", 5, 4, 12 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"NUM", 0, 5, 10 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"CHAR", 1, 5, 10 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"HLT", 2, 5, 10 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"SLA", 0, 6, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"SRA", 1, 6, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"SLAX", 2, 6, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"SRAX", 3, 6, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"SLC", 4, 6, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"SRC", 5, 6, 2 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"MOVE", 1, 7, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"LDA", 5, 8, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"LD1", 5, 9, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"LD2", 5, 10, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"LD3", 5, 11, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"LD4", 5, 12, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"LD5", 5, 13, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"LD6", 5, 14, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"LDX", 5, 15, 2 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"LDAN", 5, 16, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"LD1N", 5, 17, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"LD2N", 5, 18, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"LD3N", 5, 19, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"LD4N", 5, 20, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"LD5N", 5, 21, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"LD6N", 5, 22, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"LDXN", 5, 23, 2 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"STA", 5, 24, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ST1", 5, 25, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ST2", 5, 26, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ST3", 5, 27, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ST4", 5, 28, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ST5", 5, 29, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ST6", 5, 30, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"STX", 5, 31, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"STJ", 2, 32, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"STZ", 5, 33, 2 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"JBUS", 0, 34, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"IOC", 0, 35, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"IN", 0, 36, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"OUT", 0, 37, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JRED", 0, 38, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"JMP", 0, 39, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JSJ", 1, 39, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JOV", 2, 39, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JNOV", 3, 39, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JL", 4, 39, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JE", 5, 39, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JG", 6, 39, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JGE", 7, 39, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JNE", 8, 39, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JLE", 9, 39, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"JAN", 0, 40, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JAZ", 1, 40, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JAP", 2, 40, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JANN", 3, 40, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JANZ", 4, 40, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JANP", 5, 40, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"J1N", 0, 41, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J1Z", 1, 41, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J1P", 2, 41, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J1NN", 3, 41, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J1NZ", 4, 41, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J1NP", 5, 41, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"J2N", 0, 42, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J2Z", 1, 42, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J2P", 2, 42, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J2NN", 3, 42, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J2NZ", 4, 42, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J2NP", 5, 42, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"J3N", 0, 43, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J3Z", 1, 43, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J3P", 2, 43, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J3NN", 3, 43, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J3NZ", 4, 43, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J3NP", 5, 43, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"J4N", 0, 44, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J4Z", 1, 44, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J4P", 2, 44, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J4NN", 3, 44, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J4NZ", 4, 44, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J4NP", 5, 44, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"J5N", 0, 45, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J5Z", 1, 45, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J5P", 2, 45, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J5NN", 3, 45, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J5NZ", 4, 45, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J5NP", 5, 45, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"J6N", 0, 46, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J6Z", 1, 46, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J6P", 2, 46, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J6NN", 3, 46, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J6NZ", 4, 46, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"J6NP", 5, 46, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"JXN", 0, 47, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JXZ", 1, 47, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JXP", 2, 47, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JXNN", 3, 47, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JXNZ", 4, 47, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"JXNP", 5, 47, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"INCA", 0, 48, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"DECA", 1, 48, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ENTA", 2, 48, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ENNA", 3, 48, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"INC1", 0, 49, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"DEC1", 1, 49, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ENT1", 2, 49, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ENN1", 3, 49, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"INC2", 0, 50, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"DEC2", 1, 50, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ENT2", 2, 50, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ENN2", 3, 50, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"INC3", 0, 51, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"DEC3", 1, 51, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ENT3", 2, 51, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ENN3", 3, 51, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"INC4", 0, 52, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"DEC4", 1, 52, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ENT4", 2, 52, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ENN4", 3, 52, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"INC5", 0, 53, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"DEC5", 1, 53, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ENT5", 2, 53, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ENN5", 3, 53, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"INC6", 0, 54, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"DEC6", 1, 54, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ENT6", 2, 54, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ENN6", 3, 54, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"INCX", 0, 55, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"DECX", 1, 55, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ENTX", 2, 55, 1 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"ENNX", 3, 55, 1 });

    INSTRUCTION_LIST.push_back(InstructionInfo{"CMPA", 5, 56, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"CMP1", 5, 57, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"CMP2", 5, 58, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"CMP3", 5, 59, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"CMP4", 5, 60, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"CMP5", 5, 61, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"CMP6", 5, 62, 2 });
    INSTRUCTION_LIST.push_back(InstructionInfo{"CMPX", 5, 63, 2 });
}

void MIXMachine::prepareMIXMachine() {
    MIXMachine::makeCharTable();
    MIXMachine::makeInstructionList();
}
