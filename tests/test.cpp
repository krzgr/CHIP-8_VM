#include <gtest/gtest.h>
#include "../src/CHIP8.hpp"

class CHIP8_test : public CHIP8
{
public:
    CHIP8_test(CHIP8_Mediator& m)
        : CHIP8(m) { }
        
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

/*
TEST(chip_test, clear_screen)
{
    ASSERT_TRUE(false);
}
*/

TEST(chip_test, reset_functionality)
{
    CHIP8_Mediator m;
	CHIP8_test t(m);

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
    CHIP8_Mediator m;
	CHIP8_test t(m);
    //Calls subroutine at 0x0555
    uint8_t instr[] = { 0x25, 0x55 };
    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));
    
    //Then immediately Returns from a subroutine
    uint8_t instr2[] = { 0x00, 0xee };
    memcpy(&t.getRAM()[0] + 0x555, instr2, sizeof(instr2));

    t.clockCycle();
    ASSERT_EQ(t.getPC(), 0x555);

    ASSERT_EQ(t.getSP(), 1);

    t.clockCycle();
    ASSERT_EQ(t.getPC(), 0x202);

    ASSERT_EQ(t.getSP(), 0);
}

TEST(chip_test, jump_to_adress)
{
    CHIP8_Mediator m;
	CHIP8_test t(m);
    //Jumps to 0x0555
    uint8_t instr[] = { 0x15, 0x55 };

    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));

    t.clockCycle();
    ASSERT_EQ(t.getPC(), 0x555);
}

TEST(chip_test, skipping_instructions)
{
    CHIP8_Mediator m;
	CHIP8_test t(m);
    //Skip next instruction if Vx == NN
    uint8_t instr[] = { 0x30, 0x55, // skip if V[0] == 0x55
                        0x30, 0x00, // skip if V[0] == 0x00
                        0x00, 0x00, // empty instruction
                        0x40, 0x00, // skip if V[0] != 0x00
                        0x4f, 0xff, // skip if V[0xf] != 0xff
                        0x00, 0x00, // empty instruction
                        0x5e, 0xa0, // skip if V[0xe] == V[0xa]
                        0x00, 0x00, // empty instruction
                        0x91, 0x30 // skip if V[0x1] != V[0x3]
                      };

    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));

    t.clockCycle();
    ASSERT_EQ(t.getPC(), 0x202);

    t.clockCycle();
    ASSERT_EQ(t.getPC(), 0x206);

    t.clockCycle();
    ASSERT_EQ(t.getPC(), 0x208);

    t.clockCycle();
    ASSERT_EQ(t.getPC(), 0x20c);

    t.clockCycle();
    ASSERT_EQ(t.getPC(), 0x210);

    t.clockCycle();
    ASSERT_EQ(t.getPC(), 0x212);
}

TEST(chip_test, setting_V_register)
{
    CHIP8_Mediator m;
	CHIP8_test t(m);
    
    uint8_t instr[] = { 0x65, 0x11, // V[0x5] = 0x11
                        0x6f, 0xff  // V[0xf] = 0xff
                      };

    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));

    t.clockCycle();
    ASSERT_EQ(t.getV()[0x5], 0x11);

    t.clockCycle();
    ASSERT_EQ(t.getV()[0xf], 0xff);
}

TEST(chip_test, adding_value_to_register)
{
    CHIP8_Mediator m;
	CHIP8_test t(m);
    
    uint8_t instr[] = { 0x75, 0x11, // V[0x5] += 0x11
                        0x75, 0x22, // V[0x5] += 0x22
                        0x7f, 0x33  // V[0xf] += 0x33
                      };

    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));

    t.clockCycle();
    ASSERT_EQ(t.getV()[0x5], 0x11);

    t.clockCycle();
    ASSERT_EQ(t.getV()[0x5], 0x33);

    t.clockCycle();
    ASSERT_EQ(t.getV()[0xf], 0x33);
}

TEST(chip_test, assigning_a_registry_to_another)
{
    CHIP8_Mediator m;
	CHIP8_test t(m);
    
    uint8_t instr[] = { 0x85, 0x10, // V[0x5] = V[0x1]
                        0x8a, 0x50  // V[0xa] = V[0x5]
                      };
    
    // we write to register V[1] value 0x77
    t.getV()[0x1] = 0x77;

    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));

    t.clockCycle();
    ASSERT_EQ(t.getV()[0x5], 0x77);

    t.clockCycle();
    ASSERT_EQ(t.getV()[0xa], 0x77);
}

TEST(chip_test, bitwise_OR_operation)
{
    CHIP8_Mediator m;
	CHIP8_test t(m);
    
    uint8_t instr[] = { 0x85, 0x11, // V[0x5] |= V[0x1]
                        0x8a, 0x51  // V[0xa] |= V[0x5]
                      };
    
    // we write to register V[1] value 0x77
    t.getV()[0x1] = 0x77;

    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));

    t.clockCycle();
    ASSERT_EQ(t.getV()[0x5], 0x77);

    t.clockCycle();
    ASSERT_EQ(t.getV()[0xa], 0x77);
}

TEST(chip_test, bitwise_AND_operation)
{
    CHIP8_Mediator m;
	CHIP8_test t(m);
    
    uint8_t instr[] = { 0x85, 0x12, // V[0x5] &= V[0x1]
                        0x81, 0x12  // V[0x1] &= V[0x1]
                      };
    
    // we write to register V[1] value 0x77
    t.getV()[0x1] = 0x77;

    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));

    t.clockCycle();
    ASSERT_EQ(t.getV()[0x5], 0x00);

    t.clockCycle();
    ASSERT_EQ(t.getV()[0x1], 0x77);
}

TEST(chip_test, bitwise_XOR_operation)
{
    CHIP8_Mediator m;
	CHIP8_test t(m);
    
    uint8_t instr[] = { 0x85, 0x13, // V[0x5] ^= V[0x1]
                        0x85, 0x53  // V[0x5] ^= V[0x5]
                      };
    
    // we write to register V[1] value 0x77
    t.getV()[0x1] = 0x77;

    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));

    t.clockCycle();
    ASSERT_EQ(t.getV()[0x5], 0x77);

    t.clockCycle();
    ASSERT_EQ(t.getV()[0x5], 0x00);
}

TEST(chip_test, adding_two_registrs)
{
    CHIP8_Mediator m;
	CHIP8_test t(m);
    
    uint8_t instr[] = { 0x85, 0x14, // V[0x5] += V[0x1]
                        0x85, 0x54  // V[0x5] += V[0x5]
                      };
    
    // we write to register V[1] value 0xaa
    t.getV()[0x1] = 0xaa;

    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));

    t.clockCycle();
    ASSERT_EQ(t.getV()[0xf], 0x0);

    t.clockCycle();
    ASSERT_EQ(t.getV()[0xf], 0x1);
}

TEST(chip_test, subtracting_two_registrs)
{
    CHIP8_Mediator m;
	CHIP8_test t(m);
    
    uint8_t instr[] = { 0x85, 0x15, // V[0x5] -= V[0x1]
                        0x85, 0x15  // V[0x5] -= V[0x1]
                      };
    
    // we write to register V[0x1] value 0xaa and to V[0x2] value 0xbb
    t.getV()[0x1] = 0xaa;
    t.getV()[0x5] = 0xbb;

    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));

    t.clockCycle();
    ASSERT_EQ(t.getV()[0xf], 0x1);

    t.clockCycle();
    ASSERT_EQ(t.getV()[0xf], 0x0);
}

TEST(chip_test, bitwise_SHIFTING_RIGHT_operation)
{
    CHIP8_Mediator m;
	CHIP8_test t(m);
    
    uint8_t instr[] = { 0x85, 0x16, // V[0x5] >>= 1
                        0x85, 0xf6  // V[0x5] >>= 1
                      };
    
    // we write to register V[0x2] value 0xbb
    t.getV()[0x5] = 0x66;

    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));

    t.clockCycle();
    ASSERT_EQ(t.getV()[0xf], 0x0);

    t.clockCycle();
    ASSERT_EQ(t.getV()[0xf], 0x1);
}

TEST(chip_test, subtracting_two_registrs_2)
{
    CHIP8_Mediator m;
	CHIP8_test t(m);
    
    uint8_t instr[] = { 0x85, 0x17, // V[0x5] = V[0x1] - V[0x5]
                        0x85, 0x27  // V[0x5] = V[0x2] - V[0x5]
                      };
    
    // we write to register V[0x1] value 0xbb and to V[0x2] value 0xaa
    t.getV()[0x1] = 0xbb;
    t.getV()[0x5] = 0xaa;

    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));

    t.clockCycle();
    ASSERT_EQ(t.getV()[0xf], 0x1);

    t.clockCycle();
    ASSERT_EQ(t.getV()[0xf], 0x0);
}

TEST(chip_test, bitwise_SHIFTING_LEFT_operation)
{
    CHIP8_Mediator m;
	CHIP8_test t(m);

    uint8_t instr[] = { 0x85, 0x1e, // V[0x5] <<= 1
                        0x85, 0xfe  // V[0x5] <<= 1
                      };
    
    // we write to register V[0x2] value 0x80
    t.getV()[0x5] = 0x80;

    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));

    t.clockCycle();
    ASSERT_EQ(t.getV()[0xf], 0x1);

    t.clockCycle();
    ASSERT_EQ(t.getV()[0xf], 0x0);
}

TEST(chip_test, setting_I_register)
{
    CHIP8_Mediator m;
	CHIP8_test t(m);

    uint8_t instr[] = { 0xa5, 0x55, // I = 0x555
                        0xaf, 0xff  // I = 0xfff
                      };
    
    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));

    t.clockCycle();
    ASSERT_EQ(t.getI(), 0x555);

    t.clockCycle();
    ASSERT_EQ(t.getI(), 0xfff);
}

TEST(chip_test, jump_to_adress_plus_offset)
{
    CHIP8_Mediator m;
	CHIP8_test t(m);

    uint8_t instr[] = { 0xb5, 0x55 // PC = 0x555 + V[0]
                      };
    
    // we are writing to register V[0x0] value 0x11
    t.getV()[0x0] = 0x11;

    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));

    t.clockCycle();
    ASSERT_EQ(t.getPC(), 0x566);
}

TEST(chip_test, Vx_to_random_number)
{
    CHIP8_Mediator m;
	CHIP8_test t(m);

    uint8_t instr[] = { 0xc5, 0x0f // V[0x5] = rand() & 0xf
                      };

    memcpy(&t.getRAM()[0] + t.getPC(), instr, sizeof(instr));

    t.clockCycle();
    ASSERT_LT(t.getV()[0x5], 0x10);
}

/*
TEST(chip_test, drawing_a_sprite)
{
    ASSERT_TRUE(false);
}
*/