#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iostream>

struct Timer {
   public:
    Timer() { Start(); }

    ~Timer() {
        if (!hasBeenStopped) {
            std::cout << Stop() << " ms" << std::endl;
        }
    }

    void Start() {
        StartTimePoint = std::chrono::high_resolution_clock::now();
        hasBeenStopped = false;
    }

    // Returns time elapsed since last call of Start in milliseconds
    double Stop() {
        auto EndTimePoint = std::chrono::high_resolution_clock::now();

        auto StartTime =
            std::chrono::time_point_cast<std::chrono::microseconds>(StartTimePoint).time_since_epoch().count();
        auto EndTime = std::chrono::time_point_cast<std::chrono::microseconds>(EndTimePoint).time_since_epoch().count();

        // Unit: millisecond
        auto duration = (EndTime - StartTime) / (double)1000;

        hasBeenStopped = true;

        return duration;
    }

    // Returns time elapsed since last call of Start in microiseconds
    double StopMicroseconds() {
        auto EndTimePoint = std::chrono::high_resolution_clock::now();

        auto StartTime =
            std::chrono::time_point_cast<std::chrono::microseconds>(StartTimePoint).time_since_epoch().count();
        auto EndTime = std::chrono::time_point_cast<std::chrono::microseconds>(EndTimePoint).time_since_epoch().count();

        // Unit: microseconds
        auto duration = EndTime - StartTime;

        hasBeenStopped = true;

        return duration;
    }

   private:
    std::chrono::time_point<std::chrono::high_resolution_clock> StartTimePoint;

    bool hasBeenStopped;
};

#endif