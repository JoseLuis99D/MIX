#ifndef MIX_MIXWORD_H
#define MIX_MIXWORD_H

#define WORD_SIZE 5
#define BYTE_SIZE 6

#include <iostream>
#include <bitset>

using namespace std;

/* We choose (by convention) that 1 stands for negative while 0 stands for positive. */
enum Sign{
    Positive,
    Negative
};

/*
 * Structure of a MIX Word:
 * +-----------------------+
 * | 0 | 1 | 2 | 3 | 4 | 5 |
 * +-----------------------+
 * | - | D | D | D | D | D |
 * +-----------------------+
 *
 * Locations 1-5 are one byte (6 bits) each and contain numerical data.
 * Location 0 is a single bit and contains the sign of the word.
*/
class MIXWord {
    static string ALL_ZERO;
    static string ALL_ONE;
    static string BYTE_ZERO;
    static string BYTE_ONE;
    static int SIGN_POS;
    static int PLUS_MASK;
    static int MINUS_MASK;
    int data;

    static void MIXWordinit();

public:
    static int MaxValue();

    /* Constructors */
    MIXWord();
    explicit MIXWord(int);
    MIXWord(MIXWord&);

    /* Basic getters */
    Sign getSign() const;
    int getUnsignedValue() const;
    int getValue() const;

    /* Basic setters */
    void setSign(Sign value);
    void setUnsignedValue(int);
    void setValue(int);

    /* Field methods */
    int getField(int) const;
    int getField(int, int) const;
    void setField(int, int);
    void setField(int, int, int);

    /* Conversions */
    explicit operator int();

    /* Unary operations */
    MIXWord operator-();
    MIXWord operator++();
    MIXWord operator--();

    /* Binary operations */
    MIXWord operator+(MIXWord);
    MIXWord operator+(int);
    MIXWord operator-(MIXWord);

    /* Comparison operations */
    bool operator==(MIXWord);
    bool operator!=(MIXWord);
    bool operator>(MIXWord);
    bool operator<(MIXWord);
    bool operator>=(MIXWord);
    bool operator<=(MIXWord);

    /* Helpers */
    string to_string() const ;
    string to_instruction_string() const;
    friend ostream& operator<<(ostream&, const MIXWord&);
    char* ToByteArray();
    static MIXWord fromByteArray(char*);
};

struct MemoryCell{
    int source_location;
    int location;
    MIXWord contents;
};

#endif //MIX_MIXWORD_H
