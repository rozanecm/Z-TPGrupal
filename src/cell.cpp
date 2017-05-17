//
// Created by rodian on 13/05/17.
//

#include "cell.h"

Cell::Cell(int x, int y, int width, int lenght, std::string &kind, int factor):
            size(x, y, width, lenght), terrain(kind,factor) {}

std::string Cell::getTerrainType() const{
//    Lock l(m);
    return this->terrain.getKind();
}

int Cell::getMovementFactor() const {
//    Lock l(m);
    return this->terrain.getFactor();
}

int Cell::getXPosition() const {
    return this->size.getXPosition();
}

int Cell::getYPosition() const {
    return this->size.getYPosition();
}

Cell::~Cell() {}

bool Cell::areYouOnThisPosition(int x_pos, int y_pos) {
    return this->size.areYouOnThisPoint(x_pos, y_pos);
}
