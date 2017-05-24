#ifndef Z_TPGRUPAL_LOCK_H
#define Z_TPGRUPAL_LOCK_H


#include <mutex>

class Lock {
private:
    std::mutex &m;

public:
    explicit Lock(std::mutex  &m);

    ~Lock();
};


#endif //Z_TPGRUPAL_LOCK_H
