#include "CHIP8.hpp"

CHIP8::CHIP8()
    : RAM(4096), V(16), STACK(12)
{
    this->reset();
}

CHIP8::~CHIP8()
{
    
}

bool CHIP8::loadMemoryImage(std::string filename)
{
    std::fstream memoryImageFile(filename, std::ios::in | std::ios::binary);

    if(memoryImageFile.good() == false)
        return false;
    
    memoryImageFile.seekg(0, std::ios::end);

    const int fileLengthInBytes = memoryImageFile.tellg();

    if(fileLengthInBytes > RAM.size() - memoryImageOffset)
        return false;
    
    memoryImageFile.seekg(0, std::ios::beg);
    memoryImageFile.read((char*)RAM.data() + memoryImageOffset, fileLengthInBytes);

    return true;
}

void CHIP8::reset()
{
    std::fill(RAM.begin(), RAM.end(), 0);
    std::fill(V.begin(), V.end(), 0);
    I = 0;
    PC = 0x200;

    std::fill(STACK.begin(), STACK.end(), 0);
    SP = 0;
}

void CHIP8::run()
{
    clockCycle();
}

void CHIP8::clockCycle()
{
    const uint16_t opcode = (uint16_t(RAM.at(PC)) << 8) | uint16_t(RAM.at(PC + 1));

    switch (opcode & 0xf000)
    {
        
    }

    PC += 2;
}


uint16_t CHIP8::getNNN(uint16_t opcode)
{
    return opcode & 0xfff;
}

uint16_t CHIP8::getNN(uint16_t opcode)
{
    return opcode & 0xff;
}

uint16_t CHIP8::getN(uint16_t opcode)
{
    return opcode & 0xf;
}

uint16_t CHIP8::getX(uint16_t opcode)
{
    return (opcode >> 8) & 0xf;
}

uint16_t CHIP8::getY(uint16_t opcode)
{
    return (opcode >> 4) & 0xf;
}