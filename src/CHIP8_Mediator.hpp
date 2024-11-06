#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

#include <thread>
#include <condition_variable>
#include <atomic>
#include <vector>

#include <future>

namespace CHIP8_CONSTANTS
{
    static const int frameWidth = 64;
    static const int frameHeight = 32;

    static const int timersTickDurationInMiliseconds = 16;

    static const int keyArraySize = 16;
}


class CHIP8_Mediator
{
private:
    std::mutex mtx;
    std::condition_variable keyboardCV;
    std::condition_variable soundCV;

    std::atomic<bool> frameBufferChanged;
    std::atomic<bool> chipShouldStop;
    std::atomic<bool> soundEffect;

    std::vector<bool> keyArray;
    std::vector<std::vector<bool>> frameBuffer;

public:
    CHIP8_Mediator();
    ~CHIP8_Mediator();

    bool hasFrameBufferChanged();

    void updateFrameBuffer(const std::vector<std::vector<bool>>& newFrameBuffer);
    std::vector<std::vector<bool>> getNewFrameBuffer();

    void updateKeyArray(const std::vector<bool>& newKeyArray);

    bool isKeyPressed(uint8_t key);
    bool isKeyReleased(uint8_t key);

    uint8_t getNewKeyPress();

    void stopCHIP8();
    bool shouldCHIP8Stop();

    bool isSoundEffect();
    void setSoundEffect();
    void unsetSoundEffect();

    void waitForSoundEffect();
};