//
// Created by rodian on 15/05/17.
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
};


#endif //Z_TPGRUPAL_LOCK_H
