#include <stdlib.h>
#include "Random.h"

Random::Random() : seed((unsigned int) time(NULL)) {
}

int Random::generate() {
    return rand_r(&seed);
}
