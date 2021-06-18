#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

class CHIP8
{
public:
    static const int memoryImageOffset = 0x200;


    static uint16_t getNNN(uint16_t opcode);
    static uint16_t getNN(uint16_t opcode);
    static uint16_t getN(uint16_t opcode);

    static uint16_t getX(uint16_t opcode);
    static uint16_t getY(uint16_t opcode);

private:
    std::vector<uint8_t> RAM;
    std::vector<uint8_t> V;
    uint16_t I;
    uint16_t PC;

    std::vector<uint16_t> STACK;
    uint8_t SP;
public:
    CHIP8();
    ~CHIP8();

    bool loadMemoryImage(std::string filename);
    void reset();

    void run();

private:
    void clockCycle();
};