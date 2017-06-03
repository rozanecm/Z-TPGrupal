#include <stdlib.h>
#include "Random.h"

Random::Random() {
}

int Random::generate() {
    return rand_r(&seed);
}
