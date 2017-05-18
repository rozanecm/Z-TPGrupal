//
// Created by rodian on 13/05/17.
//

#include "Occupant.h"

Occupant::Occupant(int life, int id, Size position, Map map) : life_points(life)
        , id(id), occ_size(position), wordl_map(map) {}

int Occupant::getId() const {
    return this->id;
}

void Occupant::reduceLifeBy(int dmg) {
    this->life_points -= dmg;
}

int Occupant::getXPosition() {
    return this->occ_size.getXPosition();
}

int Occupant::getYPosition() {
    return this->occ_size.getYPosition();
}

bool Occupant::areYouAlive() {
    return (this->life_points > 0);
}

Occupant::~Occupant() {}
