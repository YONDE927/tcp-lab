#include <iostream>
#include <thread>

void func1(int x, int y, ...){ 
    return;
}

template<class F, class ...Args>
//void func2(void (*func)(Args...), Args&&... args){
void func2(F func, Args&&... args){
    std::thread work(func, 0, args...);
    work.join();
    return;
}

int main(){
    func2(func1, 3);
    return 0;
}

