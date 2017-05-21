#ifndef PROYECTO_THREAD_H
#define PROYECTO_THREAD_H


#include <thread>

class Thread {
protected:
    std::thread thread;

public:
    void start();

    virtual void run() = 0;

    void join();
};


#endif //PROYECTO_THREAD_H
