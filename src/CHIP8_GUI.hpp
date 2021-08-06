#include "CHIP8.hpp"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class CHIP8_GUI
{
public:
    static const int brickSize = 16;
private:
    CHIP8 chip8VM;
    CHIP8_Mediator mediator;
    std::thread chip8Thread;

    std::vector<std::vector<bool>> frameBuffer;
    std::vector<bool> keyArray;

    sf::RenderWindow window;
	sf::RectangleShape brick;
	sf::Event event;

    sf::Color brickColor;
public:
    CHIP8_GUI(std::string filepath);
    ~CHIP8_GUI();

    void run();
};