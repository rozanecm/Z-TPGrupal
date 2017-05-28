#ifndef Z_TPGRUPAL_THREAD_H
#define Z_TPGRUPAL_THREAD_H


#include <thread>

class Thread {
protected:
    std::thread thread;

public:
    void start();

    virtual void run() = 0;

    void join();
};


#endif //Z_TPGRUPAL_THREAD_H
