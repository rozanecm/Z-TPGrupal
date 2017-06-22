//
// Created by rodian on 22/05/17.
//

#include "teamable.h"

Teamable::Teamable(std::string team, Size size) : team(team), obj_size(size),
                                                    changed(false){}

Teamable::Teamable(Size size) : team("Neutral"), obj_size(size),
                                changed(false) {}

void Teamable::changeTeam(std::string team) {
    this->team = team;
    this->changed = true;
}

std::string Teamable::getTeam() const {
    return this->team;
}

Position Teamable::getPosition() const {
    return obj_size.getPosition();
}

bool Teamable::isThereACollision(Size& other) {
    return obj_size.isThereACollision(other);
}

Size Teamable::getSize() const {
    return this->obj_size;
}

bool Teamable::haveYouChanged() {
    bool tmp = changed;
    changed =false;
    return tmp;
}

