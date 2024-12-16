#include <iostream>
#include <chrono>

class PIDController
{
public:
    PIDController(float Kp, float Ki, float Kd, float setPoint)
        : Kp(Kp), Ki(Ki), Kd(Kd), setPoint(setPoint), integralSum(0.0), lastError(0.0)
    {
        lastTime = std::chrono::steady_clock::now();
    }

    float update(float currentPosition)
    {
        float error = setPoint - currentPosition;

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsedTime = now - lastTime;
        float deltaTime = elapsedTime.count();

        if (deltaTime <= 0.0)
            deltaTime = 1e-6;

        float derivative = (error - lastError) / deltaTime;

        integralSum += error * deltaTime;

        float output = (Kp * error) + (Ki * integralSum) + (Kd * derivative);

        lastError = error;
        lastTime = now;

        return output;
    }

private:
    float Kp, Ki, Kd;
    float setPoint;
    float integralSum;
    float lastError;
    std::chrono::steady_clock::time_point lastTime; // To calculate elapsed time
};
