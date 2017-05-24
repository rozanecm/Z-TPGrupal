//
// Created by rodian on 18/05/17.
//

#include "position.h"

Position::Position(int x, int y) : x(x), y(y) {}

int Position::getX() const {
    return this->x;
}

int Position::getY() const {
    return this->y;
}

void Position::moveTo(int x, int y) {
    this->x = x;
    this->y = y;
}
