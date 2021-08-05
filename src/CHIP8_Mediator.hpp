#include <thread>
#include <condition_variable>
#include <vector>

class CHIP8_Mediator
{
private:
    std::mutex mtx;
    std::condition_variable cv;

    std::vector<bool> keyArray;
    std::vector<std::vector<bool>> frameBuffer;

public:
    CHIP8_Mediator();
    ~CHIP8_Mediator();
};