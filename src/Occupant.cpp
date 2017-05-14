//
// Created by rodian on 13/05/17.
//

#include "Occupant.h"

Occupant::Occupant(Cell* position, int id) : position(position), id(id){}

Occupant::Occupant(int id) : id(id) {}

int Occupant::getId() const {
    return this->id;
}

Occupant::~Occupant() {}

Cell *Occupant::getPosition() {
    return this->position;
}

void Occupant::setNewPosition(Cell* position) {
    this->position = position;
}
