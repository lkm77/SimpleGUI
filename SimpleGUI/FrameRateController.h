#pragma once

#include <atomic>
#include <chrono>
#include <thread>

namespace SimpleGUI
{
    using Clock = std::chrono::high_resolution_clock;
    class FrameRateController
    {
    public:
        explicit FrameRateController(int targetFPS=30): 
            targetFrameTime_(1000.0 / targetFPS), lastTime_(Clock::now()) {}

        void waitForNextFrame()
        {
            if (!enableframeRateControl)return;
            auto now = Clock::now();
            std::chrono::duration<double, std::milli> elapsedTime = now - lastTime_;

            if (elapsedTime.count() < targetFrameTime_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(targetFrameTime_ - elapsedTime.count())));
            }

            lastTime_ = std::chrono::steady_clock::now();
        }
        void SetTargetFPS(int targetFPS) {
            targetFrameTime_ = 1000.0 / targetFPS;
        }

        void EnableFrameRateControl(bool enable)
        {
            enableframeRateControl=enable;
        }

    private:
        bool enableframeRateControl = true;
        double targetFrameTime_; // 每帧时间（毫秒）
        Clock::time_point lastTime_; // 上一帧时间
    };
}