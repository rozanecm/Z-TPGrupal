//
// Created by rodian on 13/05/17.
//

#include "cell.h"

Cell::Cell(int x, int y, std::string &kind, int factor, Occupant* ocpt):
                        x(x), y(y) ,terrain(kind,factor), occupant(ocpt){}

bool Cell::isEmpty() const {
    Lock l(m);
    return (this->occupant->getId() == -1);
}

Occupant *Cell::getOccupant() {
    Lock l(m);
    return this->occupant;
}

std::string Cell::getTerrainType() const{
    Lock l(m);
    return this->terrain.getKind();
}

int Cell::getMovementFactor() const {
    Lock l(m);
    return this->terrain.getFactor();
}

Cell::~Cell() {}
