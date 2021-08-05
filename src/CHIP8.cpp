#include "CHIP8.hpp"

CHIP8::CHIP8(CHIP8_Mediator& Mediator)
    : RAM(4096), V(16), STACK(12), mediator(Mediator),
        frameBuffer(CHIP8_CONSTANTS::frameHeight, std::vector<bool>(CHIP8_CONSTANTS::frameWidth, false)),
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
    
    memoryImageFile.unsetf(std::ios::skipws);
    
    memoryImageFile.seekg(0, memoryImageFile.end);
    const int fileLengthInBytes = memoryImageFile.tellg();
    memoryImageFile.seekg(0, memoryImageFile.beg);

    if(fileLengthInBytes > RAM.size() - memoryImageOffset)
        return false;
    
    memoryImageFile.read((char*)(RAM.data() + memoryImageOffset), fileLengthInBytes);

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

    std::vector<uint8_t> font = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    
    std::copy(font.begin(), font.end(), RAM.begin());
}

void CHIP8::run()
{
    while(true)
    {
        clockCycle();
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
                    for(auto& row : frameBuffer)
                        std::fill(row.begin(), row.end(), false);
                    mediator.updateFrameBuffer(frameBuffer);
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
        {
            uint8_t n = getN(opcode);
            uint8_t x = V[getX(opcode)];
            uint8_t y = V[getY(opcode)];

            V[0xf] = 0;

            for(uint8_t i = 0; i < n; i++)
            {
                for(uint8_t j = 0x80; j > 0; j >>= 1)
                {
                    if(frameBuffer[y][x] == true && (RAM[I + i] & j) != 0)
                        V[0xf] = 1;
                    frameBuffer[y][x] = ((int)frameBuffer[y][x] ^ ((RAM[I + i] & j) != 0 ? 1 : 0)) != 0;
                    x = (x + 1) % CHIP8_CONSTANTS::frameWidth;
                }
                x -= 8;
                y = (y + 1) % CHIP8_CONSTANTS::frameHeight;
            }

            std::cout << std::string(64, '=') << std::endl;

            for(const auto& row : frameBuffer)
            {
                for(const auto& x : row)
                    std::cout << (x == true ? '#' : ' ');
                std::cout << std::endl;
            }
                
            //std::cout << "DRAWING A SPRITE - NOT IMPLEMENTED YET!" << std::endl;
            break;
        }
        case 0xe000:
        {
            switch (opcode & 0xff)
            {
                case 0x009e:
                    if(mediator.isKeyPressed(V[getX(opcode)]))
                        PC += 2;
                    break;

                case 0x00a1:
                    if(mediator.isKeyReleased(V[getX(opcode)]))
                        PC += 2;
                    break;

                default:
                    std::cout << "OPCODE " << std::hex << (int)opcode << " DOES NOT EXISTS!" << std::endl;
                    break;
            }

            //std::cout << "KEY PRESSING EVENTS IS NOT IMPLEMENTED YET!" << std::endl;

        } break;

        case 0xf000:
        {
            switch (opcode & 0xff)
            {
                case 0x07:
                    V[getX(opcode)] = delayTimer;
                    break;
                
                case 0x0a:
                    V[getX(opcode)] = mediator.getNewKeyPress();
                    //std::cout << "KEY PRESSING - NOT IMPLEMENTED YET!" << std::endl;
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
                    I = (uint16_t)V[getX(opcode)] * (uint16_t)5;
                    //std::cout << "LOCATION OF A SPRITE - NOT IMPLEMENTED YET!" << std::endl;
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