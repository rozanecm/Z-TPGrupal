//
// Created by rodian on 13/05/17.
//

#include "Occupant.h"

Occupant::Occupant(int life, std::string type, Size position/*, Map map*/) : life_points(life)
        , type(type), occ_size(position)/*, wordl_map(map)*/ {}

std::string Occupant::getType() const {
    return this->type;
}

void Occupant::reduceLifeBy(int dmg) {
    this->life_points -= dmg;
}

bool Occupant::areYouAlive() {
    return (this->life_points > 0);
}



Occupant::~Occupant() {}

Position Occupant::getPosition() const{
    return occ_size.getPosition();
}

bool Occupant::isThereACollision(Size &other) {
    return occ_size.isThereACollision(other);
}
