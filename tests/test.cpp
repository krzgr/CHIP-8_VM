#include <gtest/gtest.h>
#include "../src/CHIP8.hpp"

class CHIP8_test : public CHIP8
{
public:
    std::vector<uint8_t>& getRAM()
    {
        return RAM;
    }

    std::vector<uint8_t>& getV()
    {
        return V;
    }

    uint16_t& getI()
    {
        return I;
    }

    uint16_t& getPC()
    {
        return PC;
    }

    std::vector<uint16_t>& getSTACK()
    {
        return STACK;
    }

    uint8_t& getSP()
    {
        return SP;
    }

    uint8_t& getDelayTimer()
    {
        return soundTimer;
    }

    uint8_t& getSoundTimer()
    {
        return soundTimer;
    }

    void clockCycle()
    {
        CHIP8::clockCycle();
    }
};


TEST(chip_test, reset_functionality)
{
    CHIP8_test t;

    for(auto& x : t.getRAM())
        ASSERT_EQ(x, 0);
    
    for(auto& x : t.getV())
        ASSERT_EQ(x, 0);
    
    ASSERT_EQ(t.getI(), 0);
    ASSERT_EQ(t.getPC(), 0x200);

    ASSERT_EQ(t.getSP(), 0);

    for(auto& x : t.getSTACK())
        ASSERT_EQ(x, 0);
    
    ASSERT_EQ(t.getDelayTimer(), 0);
    ASSERT_EQ(t.getSoundTimer(), 0);
}

TEST(chip_test, check_subroutines)
{
    CHIP8_test t;
    //Calls subroutine at 0x0555
    uint8_t instr[] = { 0x25, 0x55 };
    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));
    
    //Then immediately Returns from a subroutine
    uint8_t instr2[] = { 0x00, 0xee };
    memcpy(&t.getRAM()[0] + 0x555, instr2, sizeof(instr2));

    t.clockCycle();
    ASSERT_EQ(t.getPC(), 0x555);

    t.clockCycle();
    ASSERT_EQ(t.getPC(), 0x202);
}