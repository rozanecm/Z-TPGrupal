#ifndef Z_TPGRUPAL_RANDOM_H
#define Z_TPGRUPAL_RANDOM_H

#include <ctime>

/* Random number generator */
class Random {
    unsigned int seed = (unsigned int) time(NULL);
public:
    Random();
    int generate();
};


#endif //Z_TPGRUPAL_RANDOM_H
