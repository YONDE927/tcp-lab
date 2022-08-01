#pragma once
#include <chrono>

using namespace std::chrono;

class Timer {
    private:
        high_resolution_clock::time_point tp;
        high_resolution_clock::time_point lp;
    public:
        enum Type {
            NANO,
            MICRO,
            MILL,
            SEC
        };
        void start();
        void lap();
        double check(Timer::Type type = NANO);
        void pcheck();
};
