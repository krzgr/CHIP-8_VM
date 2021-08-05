#include <iostream>
#include "CHIP8.hpp"

int main()
{
    CHIP8 vm;
    if(vm.loadMemoryImage("../res/pong.rom") == false)
        return -1;
    else
        vm.run();
    return 0;
}