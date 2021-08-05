#include "CHIP8.hpp"

CHIP8::CHIP8()
    : RAM(4096), V(16), STACK(12), 
        rng(std::chrono::high_resolution_clock::now().time_since_epoch().count())
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

    delayTimer = 0;
    soundTimer = 0;
}

void CHIP8::run()
{
    while(true)
    {
        clockCycle();
        std::cin.get();
    }
}

void CHIP8::clockCycle()
{
    const uint16_t opcode = (uint16_t(RAM.at(PC)) << 8) | uint16_t(RAM.at(PC + 1));

    switch (opcode & 0xf000)
    {
        case 0x0000:
        {
            switch (opcode)
            {
                case 0x00e0: //Clears the screen
                    std::cout << "CLEARING DISPLAY NOT IMPLEMENTED" << std::endl;
                    break;

                case 0x00ee: //Returns from a subroutine
                    PC = STACK.at(SP - 1);
                    SP--;
                    break;

                default: //Calls machine code routine at address NNN
                    std::cout << "NOT IMPLEMENTED" << std::endl;
            }
        } break;

        case 0x1000:
            PC = getNNN(opcode) - 2;
            break;

        case 0x2000:
            STACK.at(SP) = PC;
            SP++;
            PC = getNNN(opcode) - 2;
            break;

        case 0x3000:
            if(V[getX(opcode)] == getNN(opcode))
                PC += 2;
            break;
        
        case 0x4000:
            if(V[getX(opcode)] != getNN(opcode))
                PC += 2;
            break;
        
        case 0x5000:
            if(V[getX(opcode)] == V[getY(opcode)])
                PC += 2;
            break;
        
        case 0x6000:
            V[getX(opcode)] = getNN(opcode);
            break;
        
        case 0x7000:
            V[getX(opcode)] += getNN(opcode);
            break;
        
        case 0x8000:
        {
            switch (opcode & 0xf)
            {
                case 0x0:
                    V[getX(opcode)] =  V[getY(opcode)];
                    break;
                
                case 0x1:
                    V[getX(opcode)] |=  V[getY(opcode)];
                    break;
                
                case 0x2:
                    V[getX(opcode)] &=  V[getY(opcode)];
                    break;

                case 0x3:
                    V[getX(opcode)] ^=  V[getY(opcode)];
                    break;
                
                case 0x4:
                    V[0xf] = (int)V[getX(opcode)] + (int)V[getY(opcode)] > 0xff ? 1 : 0;
                    V[getX(opcode)] += V[getY(opcode)];
                    break;
                
                case 0x5:
                    V[0xf] = V[getX(opcode)] > V[getY(opcode)] ? 1 : 0;
                    V[getX(opcode)] -= V[getY(opcode)];
                    break;
                
                case 0x6:
                    V[0xf] = V[getX(opcode)] & 0x1;
                    V[getX(opcode)] >>= 1;
                    break;
                
                case 0x7:
                    V[0xf] = V[getY(opcode)] > V[getX(opcode)] ? 1 : 0;
                    V[getX(opcode)] = V[getY(opcode)] - V[getX(opcode)];
                    break;
                
                case 0xe:
                    V[0xf] = V[getX(opcode)] >> 7;
                    V[getX(opcode)] <<= 1;
                    break;
                
                default:
                    std::cout << "OPCODE " << std::hex << (int)opcode << " DOES NOT EXISTS!" << std::endl;
            }
        } break;

        case 0x9000:
        {
            if((opcode & 0x1) == 0)
            {
                if(V[getX(opcode)] != V[getY(opcode)])
                    PC += 2;
            }
            else
                std::cout << "OPCODE " << std::hex << (int)opcode << " DOES NOT EXISTS!" << std::endl;
        } break;
            
        case 0xa000:
            I = getNNN(opcode);
            break;

        case 0xb000:
            PC = (uint16_t)V[0] + getNNN(opcode) - (uint16_t)2;
            break;

        case 0xc000:
            V[getX(opcode)] = (uint16_t)rng() & getNN(opcode);
            break;
        
        case 0xd000:
            std::cout << "DRAWING A SPRITE - NOT IMPLEMENTED YET!" << std::endl;
            break;

        case 0xe000:
        {
                uint8_t delay;

        } break;

        case 0xf000:
        {
            switch (opcode & 0xff)
            {
                case 0x07:
                    V[getX(opcode)] = delayTimer;
                    break;
                
                case 0x0a:
                    std::cout << "KEY PRESSING - NOT IMPLEMENTED YET!" << std::endl;
                    break;
                
                case 0x15:
                    delayTimer = V[getX(opcode)];
                    break;
                
                case 0x18:
                    soundTimer = V[getX(opcode)];
                    break;
                
                case 0x1e:
                    I += V[getX(opcode)];
                    break;
                
                case 0x29:
                    std::cout << "LOCATION OF A SPRITE - NOT IMPLEMENTED YET!" << std::endl;
                    break;
                
                case 0x33:
                    RAM.at(I) = V[getX(opcode)] / 100;
                    RAM.at(I + 1) = (V[getX(opcode)] / 10) % 10;
                    RAM.at(I + 2) = V[getX(opcode)] % 10;
                    break;
                
                case 0x55:
                    for(uint16_t i = 0; i <= getX(opcode); i++)
                        RAM.at(I + i) = V[i];
                    break;
                
                case 0x65:
                    for(uint16_t i = 0; i <= getX(opcode); i++)
                        V[i] = RAM.at(I + i);
                    break;
                
                default:
                    std::cout << "OPCODE " << std::hex << (int)opcode << " DOES NOT EXISTS!" << std::endl;
            }
        } break;
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