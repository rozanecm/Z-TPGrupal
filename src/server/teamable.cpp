//
// Created by rodian on 22/05/17.
//

#include "teamable.h"

Teamable::Teamable(std::string team, Size size) : team(team), obj_size(size) {}

Teamable::Teamable(Size size) : team("Neutral"), obj_size(size) {}

void Teamable::changeTeam(std::string team) {
    this->team = team;
}

std::string Teamable::getTeam() const {
    return this->team;
}

Position Teamable::getPosition() {
    return obj_size.getPosition();
}

bool Teamable::isThereACollision(Size& other) {
    return obj_size.isThereACollision(other);
}

Size Teamable::getSize() {
    return this->obj_size;
}

