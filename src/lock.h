//
// Created by rodian on 14/05/17.
//

#ifndef Z_TPGRUPAL_LOCK_H
#define Z_TPGRUPAL_LOCK_H

#include <mutex>

class Lock {
private:
    std::mutex &mute;

public:
    // Locks the mutex
    explicit Lock(std::mutex &mute);

    // Frees the mutex
    ~Lock();

private:
    Lock(const Lock&) = delete;
    Lock& operator=(const Lock&) = delete;
    Lock(Lock&&) = delete;
    Lock& operator=(Lock&&) = delete;
};


#endif //Z_TPGRUPAL_LOCK_H
