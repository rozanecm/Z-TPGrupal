//
// Created by rodian on 13/05/17.
//

#include "Occupant.h"

Occupant::Occupant(int life, int id, Size position) : life_points(life), id(id),
                                                            position(position){}

int Occupant::getId() const {
    return this->id;
}
void Occupant::setNewPosition(int x, int y) {
    this->position.moveTo(x,y);
}

void Occupant::reduceLifeBy(int dmg) {
    this->life_points -= dmg;
}

int Occupant::getXPosition() {
    return this->position.getXPosition();
}

int Occupant::getYPosition() {
    return this->position.getYPosition();
}

bool Occupant::areYouAlive() {
    return (this->life_points > 0);
}

Occupant::~Occupant() {}
