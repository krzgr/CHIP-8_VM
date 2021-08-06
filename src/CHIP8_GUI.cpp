#include "CHIP8_GUI.hpp"

CHIP8_GUI::CHIP8_GUI(std::string filepath)
    : mediator(), chip8VM(mediator), frameBuffer(CHIP8_CONSTANTS::frameHeight,
        std::vector<bool>(CHIP8_CONSTANTS::frameWidth, false)), 
        keyArray(CHIP8_CONSTANTS::keyArraySize, false),
        brick(sf::Vector2f(brickSize, brickSize)),
        brickColor(sf::Color(66, 253, 110))
{

    if(chip8VM.loadMemoryImage(filepath))
    {
        chip8Thread = std::thread([this](){
            chip8VM.run();
        });
    }
    else
        std::cout << "UNABLE TO OPEN A FILE!" << std::endl;    
    
    brick.setFillColor(brickColor);
}

CHIP8_GUI::~CHIP8_GUI()
{
    mediator.stopCHIP8();
    if(chip8Thread.joinable())
        chip8Thread.join();
}

void CHIP8_GUI::run()
{
    window.create(sf::VideoMode(CHIP8_CONSTANTS::frameWidth * brickSize,
                    CHIP8_CONSTANTS::frameHeight * brickSize),
                    "CHIP8 v1.0", sf::Style::Titlebar | sf::Style::Close);

    window.setFramerateLimit(100);
    
    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            //  1	2	3	C
            //  4	5	6	D
            //  7	8	9	E
            //  A	0	B	F
            // Keyboard
            if (event.type == sf::Event::Closed)
            {
                mediator.stopCHIP8();
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::Num1:
                        keyArray[0x1] = true; break;

                    case sf::Keyboard::Num2:
                        keyArray[0x2] = true; break;

                    case sf::Keyboard::Num3:
                        keyArray[0x3] = true; break;

                    case sf::Keyboard::Num4:
                        keyArray[0xc] = true; break;

                    case sf::Keyboard::Q:
                        keyArray[0x4] = true; break;

                    case sf::Keyboard::W:
                        keyArray[0x5] = true; break;
                    
                    case sf::Keyboard::E:
                        keyArray[0x6] = true; break;

                    case sf::Keyboard::R:
                        keyArray[0xd] = true; break;
                    
                    case sf::Keyboard::A:
                        keyArray[0x7] = true; break;
                    
                    case sf::Keyboard::S:
                        keyArray[0x8] = true; break;
                    
                    case sf::Keyboard::D:
                        keyArray[0x9] = true; break;
                    
                    case sf::Keyboard::F:
                        keyArray[0xe] = true; break;
                    
                    case sf::Keyboard::Z:
                        keyArray[0xa] = true; break;
                    
                    case sf::Keyboard::X:
                        keyArray[0x0] = true; break;
                    
                    case sf::Keyboard::C:
                        keyArray[0xb] = true; break;
                    
                    case sf::Keyboard::V:
                        keyArray[0xf] = true; break;
                    default:
                        break;
                }
                mediator.updateKeyArray(keyArray);
            }
            else if (event.type == sf::Event::KeyReleased)
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::Num1:
                        keyArray[0x1] = false; break;

                    case sf::Keyboard::Num2:
                        keyArray[0x2] = false; break;

                    case sf::Keyboard::Num3:
                        keyArray[0x3] = false; break;

                    case sf::Keyboard::Num4:
                        keyArray[0xc] = false; break;

                    case sf::Keyboard::Q:
                        keyArray[0x4] = false; break;

                    case sf::Keyboard::W:
                        keyArray[0x5] = false; break;
                    
                    case sf::Keyboard::E:
                        keyArray[0x6] = false; break;

                    case sf::Keyboard::R:
                        keyArray[0xd] = false; break;
                    
                    case sf::Keyboard::A:
                        keyArray[0x7] = false; break;
                    
                    case sf::Keyboard::S:
                        keyArray[0x8] = false; break;
                    
                    case sf::Keyboard::D:
                        keyArray[0x9] = false; break;
                    
                    case sf::Keyboard::F:
                        keyArray[0xe] = false; break;
                    
                    case sf::Keyboard::Z:
                        keyArray[0xa] = false; break;
                    
                    case sf::Keyboard::X:
                        keyArray[0x0] = false; break;
                    
                    case sf::Keyboard::C:
                        keyArray[0xb] = false; break;
                    
                    case sf::Keyboard::V:
                        keyArray[0xf] = false; break;
                    default:
                        break;
                }
                mediator.updateKeyArray(keyArray);
            }
        }

        //framebuffer changed?
        if(mediator.hasFrameBufferChanged())
            frameBuffer = mediator.getNewFrameBuffer();

        //beep...
        if(mediator.isSoundEffect())
        {
            //beep...
        }

        //drawing
        window.clear(sf::Color::Black);

        brick.setPosition(sf::Vector2f(0.0f, 0.0f));
        for(auto& row : frameBuffer)
        {
            for(auto cell : row)
            {
                if(cell)
                    window.draw(brick);
                brick.move(sf::Vector2f(brickSize, 0.0f));
            }

            brick.setPosition(sf::Vector2f(0.0f, brick.getPosition().y + brickSize));
        }

        window.display();
    }

}