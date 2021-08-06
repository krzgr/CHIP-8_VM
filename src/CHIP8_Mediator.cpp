#include "CHIP8_Mediator.hpp"

CHIP8_Mediator::CHIP8_Mediator()
    : keyArray(CHIP8_CONSTANTS::keyArraySize, false), frameBufferChanged(false),
    frameBuffer(CHIP8_CONSTANTS::frameHeight, std::vector<bool>(CHIP8_CONSTANTS::frameWidth, false)),
    chipShouldStop(false)
{
    
}

CHIP8_Mediator::~CHIP8_Mediator()
{
    
}

bool CHIP8_Mediator::hasFrameBufferChanged()
{
    return frameBufferChanged.load();
}

void CHIP8_Mediator::updateFrameBuffer(const std::vector<std::vector<bool>>& newFrameBuffer)
{
    std::unique_lock<std::mutex> lck{mtx};
    frameBuffer = newFrameBuffer;
    frameBufferChanged.store(true);
}

std::vector<std::vector<bool>> CHIP8_Mediator::getNewFrameBuffer()
{
    std::unique_lock<std::mutex> lck{mtx};
    frameBufferChanged.store(false);
    return frameBuffer;
}

void CHIP8_Mediator::updateKeyArray(const std::vector<bool>& newKeyArray)
{
    {
        std::unique_lock<std::mutex> lck{mtx};
        keyArray = newKeyArray;
    }
    cv.notify_all();
}

bool CHIP8_Mediator::isKeyPressed(uint8_t key)
{
    std::unique_lock<std::mutex> lck{mtx};
    if(key > 16)
    {
        std::cout << "KEY CODE IS GREATER THAN 16!" << std::endl;
        return false;
    }
    else
        return keyArray.at(key);
}

bool CHIP8_Mediator::isKeyReleased(uint8_t key)
{
    return !isKeyPressed(key);
}

uint8_t CHIP8_Mediator::getNewKeyPress()
{
    std::unique_lock<std::mutex> lck{mtx};
    cv.wait(lck, [this](){
        return std::find(keyArray.begin(), keyArray.end(), true) != keyArray.end();
    });

    for(int i = 0; i < keyArray.size(); i++)
        if(keyArray[i])
            return i;
    
    std::cout << "SOMETHING WITH KEY PRESS WENT WRONG!" << std::endl;
    return 0;
}

void CHIP8_Mediator::stopCHIP8()
{
    {
        chipShouldStop.store(true);
        std::unique_lock<std::mutex> lck{mtx};
        keyArray[0] = true;
    }
    cv.notify_all();
}

bool CHIP8_Mediator::shouldCHIP8Stop()
{
    return chipShouldStop.load();
}