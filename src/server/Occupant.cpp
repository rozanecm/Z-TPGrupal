//
// Created by rodian on 13/05/17.
//

#include "Occupant.h"

Occupant::Occupant(int life, std::string type, Size position): life_points(life)
        , type(type), occ_size(position), damage_recv(0) {}

std::string Occupant::getType() const {
    return this->type;
}

void Occupant::reduceLifeBy(int dmg) {
    this->damage_recv += dmg;
}

bool Occupant::areYouAlive() {
    return ((life_points - damage_recv) > 0);
}



Occupant::~Occupant() {}

Position Occupant::getPosition() const{
    return occ_size.getPosition();
}

bool Occupant::isThereACollision(Size &other) {
    return occ_size.isThereACollision(other);
}

int Occupant::getLifeLeft() {
    return life_points - damage_recv;
}
