#include <iostream>
#include <chrono>

class PIDController
{
private:
    float Kp, Ki, Kd;                              // PID constants
    float setPoint;                                // Desired set point
    float integralSum;                             // Sum of errors (Integral term)
    float lastError;                               // Previous error (used for Derivative term)
    std::chrono::steady_clock::time_point lastTime; // To calculate elapsed time

public:
    PIDController(float Kp, float Ki, float Kd, float setPoint)
        : Kp(Kp), Ki(Ki), Kd(Kd), setPoint(setPoint), integralSum(0.0), lastError(0.0)
    {
        lastTime = std::chrono::steady_clock::now(); // Initialize the timer
    }

    float update(float currentPosition)
    {
        // Calculate the error
        float error = setPoint - currentPosition;

        // Get the elapsed time in seconds
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsedTime = now - lastTime;
        float deltaTime = elapsedTime.count();

        if (deltaTime <= 0.0)
            deltaTime = 1e-6; // Avoid divide-by-zero errors

        // Calculate the derivative (rate of change of error)
        float derivative = (error - lastError) / deltaTime;

        // Update the integral (sum of error over time)
        integralSum += error * deltaTime;

        // Calculate the PID output
        float output = (Kp * error) + (Ki * integralSum) + (Kd * derivative);

        // Update the last error and last time
        lastError = error;
        lastTime = now;

        return output; // Return the correction to be applied
    }
};
