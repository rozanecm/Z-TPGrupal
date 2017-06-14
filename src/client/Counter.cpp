#include "Counter.h"

void Counter::initialize(unsigned long maxSize) {
    this->maxSize = maxSize;
}

void Counter::updateCounter() {
    counter == maxSize - 1 ? (counter = 0) : (counter++);
}

unsigned short Counter::getCounter() {
    return counter;
}
