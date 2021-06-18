#include <iostream>
#include "CHIP8.hpp"

int main()
{
    CHIP8 vm;
    if(vm.loadMemoryImage("pong.rom") == false)
        return -1;
    else
        vm.run();
    return 0;
}