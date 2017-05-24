//
// Created by rodian on 15/05/17.
//

#include "lock.h"

Lock::Lock(std::mutex &mute) : mute(mute) {
    this->mute.lock();
}

Lock::~Lock() {
    this->mute.unlock();
}
