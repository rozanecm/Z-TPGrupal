#ifndef Z_TPGRUPAL_RANDOM_H
#define Z_TPGRUPAL_RANDOM_H

#include <ctime>

/* Random number generator */
class Random {
    unsigned int seed = (unsigned int) time(NULL);
public:
    Random();

    /* Returns a random int from 0 to INT_MAX */
    int generate();
};


#endif //Z_TPGRUPAL_RANDOM_H
