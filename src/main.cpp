#include "CHIP8_GUI.hpp"

int main(int argc, char **argv)
{
    if(argc != 2)
        std::cout << "Usage: CHIP-8_VM.exe [FILE]" << std::endl;
    else
    {
        CHIP8_GUI gui(argv[1]);
        gui.run();
    }
}