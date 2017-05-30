#include "Thread.h"

void Thread::start() {
    thread = std::thread(&Thread::run, this);
}

void Thread::join() {
    this->thread.join();
}
