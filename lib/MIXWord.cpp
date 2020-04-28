#include <string>
#include <cmath>
#include <bitset>
#include <cstdio>
#include "../MIX/MIXWord.h"

void MIXWord::MIXWordinit(){
    // BYTE_ZERO
    BYTE_ZERO = "";
    for (int i=0; i<BYTE_SIZE; i++)
        BYTE_ZERO += "0";
    // BYTE_ONE
    BYTE_ONE = "";
    for (int i=0; i<BYTE_SIZE; i++)
        BYTE_ONE += "1";
    // ALL_ZERO
    ALL_ZERO = "";
    for (int i=0; i<WORD_SIZE; i++)
        ALL_ZERO += BYTE_ZERO;
    // ALL_ONE
    ALL_ONE = "";
    for (int i=0; i<WORD_SIZE; i++)
        ALL_ONE += BYTE_ONE;

    SIGN_POS = MINUS_MASK = stoi("1" + ALL_ZERO, nullptr, 2);
    PLUS_MASK = stoi("0" + ALL_ONE, nullptr, 2);
}

MIXWord::MIXWord() {
    setValue(0);
}

MIXWord::MIXWord(int value) {
    setValue(value);
}

MIXWord::MIXWord(MIXWord& word) {
    setValue(word.getValue());
}

int MIXWord::MaxValue(){
    string one = "111111";
    return stoi(one + one + one + one + one, nullptr, 2);
}

Sign MIXWord::getSign() const {
    if ((data & SIGN_POS) == SIGN_POS)
        return Negative;
    else
        return Positive;
}

int MIXWord::getUnsignedValue() const {
    return data & stoi(ALL_ONE, nullptr, 2);
}

int MIXWord::getValue() const {
    return (data & SIGN_POS) == SIGN_POS ? -getUnsignedValue() : getUnsignedValue();
}

void MIXWord::setSign(Sign value){
    if (value == Positive)
        data &= PLUS_MASK;
    else
        data |= MINUS_MASK;
}

void MIXWord::setUnsignedValue(int value){
    if (value > stoi(ALL_ONE, nullptr, 2))
        throw "Value too large to fit in word";
    data &= stoi("1" + ALL_ZERO, nullptr, 2);
    data |= value;
}

void MIXWord::setValue(int value){
    setUnsignedValue(abs(value));
    setSign(Positive);
    if(value < 0)
        setSign(Negative);
}

int MIXWord::getField(int index) const {
    if (index < 0 || index > WORD_SIZE)
        throw "Invalid word index.";
    if (index == 0)
        return getSign() == Positive ? 0 : 1;
    string strBitmask = BYTE_ONE;
    for(int i=0; i<(WORD_SIZE-i); i++)
        strBitmask += BYTE_ZERO;
    int bitmask = stoi(strBitmask, nullptr, 2);
    return (data & bitmask) >> ((WORD_SIZE - index) * BYTE_SIZE);
}

int MIXWord::getField(int l, int r) const {
    if(l > r){
        int t = r;
        r = l;
        l = t;
    }

    if (l < 0 || l > WORD_SIZE)
        throw "Invalid word index (left)";
    if (r < 0 || r > WORD_SIZE)
        throw "Invalid word index (right).";

    if(r == 0)
        return getField(0);

    bool useSign = false;
    if(l == 0){
        l++;
        useSign = true;
    }

    int leftCount = l - 1;
    int midCount = 1 + (r - l);
    int rightCount = WORD_SIZE - r;

    string strBitmask = "";
    for(int i=0; i<leftCount; i++)
        strBitmask += BYTE_ZERO;
    for(int i=0; i<midCount; i++)
        strBitmask += BYTE_ONE;
    for(int i=0; i<rightCount; i++)
        strBitmask += BYTE_ZERO;
    int bitmask = stoi(strBitmask, nullptr, 2);

    int result = (data & bitmask) >> (rightCount*BYTE_SIZE);

    if (useSign && getSign() == Negative)
        return -result;
    return result;
}

void MIXWord::setField(int index, int value) {
    if (index < 0 || index > WORD_SIZE)
        throw "Invalid word index";
    if (index == 0){
        if(value == 1)
            setSign(Negative);
        else if (value == 0)
            setSign(Positive);
        else
            throw "Invalid sign value";
    }
    else{
        int leftCount = index - 1;
        int rightCount = WORD_SIZE - index;

        string strMaskClear = "";
        for(int i=0; i<leftCount; i++)
            strMaskClear += BYTE_ONE;
        strMaskClear += BYTE_ZERO;
        for(int i=0; i<rightCount; i++)
            strMaskClear += BYTE_ONE;

        int bitmaskClear = stoi("1" + strMaskClear, nullptr, 2);

        string valString = bitset<32>(abs(value)).to_string();
        valString = valString.substr(32-(int) ceil(log2(abs(value))), 32);
        for(int i=0; i<BYTE_SIZE-valString.length(); i++)
            valString = "0" + valString;

        string strBitmaskSet = "0";
        for(int i=0; i<leftCount; i++)
            strBitmaskSet += BYTE_ZERO;
        strBitmaskSet += valString;
        for(int i=0; i<rightCount; i++)
            strBitmaskSet += BYTE_ZERO;
        int bitmaskSet = stoi(strBitmaskSet, nullptr, 2);

        data &= bitmaskClear;
        data |= bitmaskSet;
    }
}

void MIXWord::setField(int l, int r, int value) {
    if(l > r){
        int t = r;
        r = l;
        l = t;
    }
    if(l < 0 || l > WORD_SIZE)
        throw "Invalid word index (left)";
    if(r < 0 || r > WORD_SIZE)
        throw "Invalid word index (right)";
    if(r == 0){
        setField(0, value);
        return;
    }

    bool useSign = false;
    if(l == 0){
        l++;
        useSign = true;
    }

    int leftCount = l - 1;
    int midCount = 1 + (r - l);
    int rightCount = WORD_SIZE - r;

    string strBitmaskClear = "1";
    for(int i=0; i<leftCount; i++)
        strBitmaskClear += BYTE_ONE;
    for(int i=0; i<midCount; i++)
        strBitmaskClear += BYTE_ZERO;
    for(int i=0; i<rightCount; i++)
        strBitmaskClear += BYTE_ONE;

    int bitmaskClear = stoi(strBitmaskClear, nullptr, 2);

    string valString = bitset<32>(abs(value)).to_string();
    valString = valString.substr(32-(int) ceil(log2(abs(value))), 32);
    for(int i=0; i< BYTE_SIZE*midCount - valString.length(); i++)
        valString = "0" + valString;

    string strBitmaskSet = "0";
    for(int i=0; i<leftCount; i++)
        strBitmaskClear += BYTE_ZERO;
    strBitmaskSet += valString;
    for(int i=0; i<rightCount; i++)
        strBitmaskSet += BYTE_ZERO;
    int bitmaskSet = stoi(strBitmaskSet, nullptr, 2);

    data &= bitmaskClear;
    data |= bitmaskSet;

    if(useSign){
        data &= PLUS_MASK;
        setSign(value<0 ? Negative : Positive);
    }
}

MIXWord::operator int() {
    return getValue();
}

MIXWord MIXWord::operator-() {
    MIXWord result(-getValue());
    return result;
}

MIXWord MIXWord::operator++() {
    MIXWord result(getValue() + 1);
    return result;
}

MIXWord MIXWord::operator--() {
    MIXWord result(getValue() - 1);
    return result;
}

MIXWord MIXWord::operator+(MIXWord word) {
    MIXWord result(getValue() + word.getValue());
    return result;
}

MIXWord MIXWord::operator+(int num) {
    MIXWord word(num);
    MIXWord result(getValue() + word.getValue());
    return result;
}

MIXWord MIXWord::operator-(MIXWord word) {
    MIXWord result(getValue() - word.getValue());
    return result;
}

bool MIXWord::operator==(MIXWord word) {
    return getValue() == word.getValue();
}

bool MIXWord::operator!=(MIXWord word) {
    return getValue() != word.getValue();
}

bool MIXWord::operator>(MIXWord word) {
    return getValue() > word.getValue();
}

bool MIXWord::operator<(MIXWord word) {
    return getValue() < word.getValue();
}

bool MIXWord::operator>=(MIXWord word) {
    return getValue() >= word.getValue();
}

bool MIXWord::operator<=(MIXWord word) {
    return getValue() <= word.getValue();
}

string MIXWord::to_string() const {
    char c_string[20];
    sprintf(c_string, "[%c|%2d|%2d|%2d|%2d|%2d|]",
            getSign()==Positive ? '+':'-',
            getField(1), getField(2), getField(3), getField(4), getField(5));
    return string(c_string);
}

ostream &operator<<(ostream &os, const MIXWord &word) {
    os << word.to_string();
    return os;
}

char *MIXWord::ToByteArray() {
    char *bytes;
    char = new byte[6];
    for (int i=0; i<6; i++)
        bytes[i] = getField(i);
    return bytes;
}

MIXWord MIXWord::fromByteArray(char *bytes) {
    MIXWord word;
    for (int i=0; i<6; i++)
        word.setField(i, bytes[i]);
    return word;
}

string MIXWord::to_instruction_string() const {
    char c_string[19];
    sprintf(c_string, "[%c|%4d|%2d|%2d|%2d|]",
            getSign()==Positive ? '+':'-',
            getField(1,2), getField(3), getField(4), getField(5));
    return string(c_string);
}
