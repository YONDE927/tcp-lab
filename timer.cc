#include <iostream>
#include <chrono>
#include "timer.h"

using namespace std::chrono;

void Timer::start(){
    tp = high_resolution_clock::now();
}

void Timer::lap(){
    lp = high_resolution_clock::now(); 
}

double Timer::check(Timer::Type type){
    double i;
    nanoseconds ns{0};
    microseconds mcs{0};
    milliseconds ms{0};
    seconds s{0};
    switch(type){
        case NANO:
            ns = lp - tp;
            i = ns.count();
            break;
        case MICRO:
            mcs = duration_cast<microseconds>(lp - tp);
            i = mcs.count();
            break;
        case MILL:
            ms = duration_cast<milliseconds>(lp - tp);
            i = ms.count();
            break;
        case SEC:
            s = duration_cast<seconds>(lp - tp);
            i = s.count();
            break;
        default:
            i = 0;
            break;
    }
    return i;
}

void Timer::pcheck(){
    std::cout << check(NANO) << " nanoseconds" << std::endl;
    std::cout << check(MICRO) << " microseconds" << std::endl;
    std::cout << check(MILL) << " milliseconds" << std::endl;
    std::cout << check(SEC) << " seconds" << std::endl;
}
