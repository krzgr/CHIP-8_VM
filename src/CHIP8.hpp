#include "CHIP8_Mediator.hpp"

class CHIP8
{
public:
    static const int memoryImageOffset = 0x200;

    static uint16_t getNNN(uint16_t opcode);
    static uint16_t getNN(uint16_t opcode);
    static uint16_t getN(uint16_t opcode);

    static uint16_t getX(uint16_t opcode);
    static uint16_t getY(uint16_t opcode);

protected:
    std::vector<uint8_t> RAM;
    std::vector<uint8_t> V;
    uint16_t I;
    uint16_t PC;

    std::vector<uint16_t> STACK;
    uint8_t SP;

    uint8_t delayTimer;
    uint8_t soundTimer;

    std::mt19937 rng;

    std::vector<std::vector<bool>> frameBuffer;
    CHIP8_Mediator& mediator;
public:
    CHIP8(CHIP8_Mediator& Mediator);
    ~CHIP8();

    bool loadMemoryImage(std::string filename);
    void reset();

    void run();

protected:
    void clockCycle();
};