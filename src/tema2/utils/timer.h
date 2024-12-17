#include <iostream>
#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>
#include <thread>

class Timer
{
public:
    Timer() : running(false), remainingTime(0) {}

    void start(int minutes, int seconds)
    {
        if (!running)
        {
            running = true;
            remainingTime = minutes * 60 + seconds;
            startTime = std::chrono::steady_clock::now();
        }
    }

    void stop()
    {
        if (running)
        {
            auto now = std::chrono::steady_clock::now();
            int elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
            remainingTime = std::max(0, remainingTime - elapsed);
            running = false;
        }
    }

    void reset()
    {
        running = false;
        remainingTime = 0;
    }

    bool ended()
    {
        return remainingTime <= 0;
    }

    std::string getTime() const
    {
        int totalSeconds = remainingTime;

        if (running)
        {
            auto now = std::chrono::steady_clock::now();
            int elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
            totalSeconds = std::max(0, remainingTime - elapsed);
        }

        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;

        std::string secondsString = std::to_string(seconds);
        if (seconds < 10)
            secondsString = "0" + secondsString;

        return std::to_string(minutes) + ":" + secondsString;
    }

private:
    bool running;
    std::chrono::steady_clock::time_point startTime;
    int remainingTime;
};
