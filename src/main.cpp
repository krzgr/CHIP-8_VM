#include "CHIP8.hpp"

int main()
{
    CHIP8_Mediator mediator;
    CHIP8 vm(mediator);
    if(vm.loadMemoryImage("../res/pong.rom") == false)
    //if(vm.loadMemoryImage("C:/Users/kgrun/Desktop/nowe_okienka/CHIP-8_VM/res/IBM_Logo.ch8") == false)
        return -1;
    else
        vm.run();
    return 0;
}