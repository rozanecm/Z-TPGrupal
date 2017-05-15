//
// Created by rodian on 13/05/17.
//

#include "cell.h"

Cell::Cell(int x, int y, std::string &kind, int factor, Occupant* ocpt):
                        x(x), y(y) ,terrain(kind,factor), occupant(ocpt){}

bool Cell::isEmpty() const {
    return (this->occupant->getId() == -1);
}

Occupant *Cell::getOccupant() {
    return this->occupant;
}

Cell::~Cell() {}

std::string Cell::getTerrainType() const{
    return this->terrain.getKind();
}

int Cell::getMovementFactor() const {
    return this->terrain.getFactor();
}
