#include "Unit.h"
#include <iostream>

#define IMG_SIZE_IN_PX 16
#define TEAM_NEUTRAL "None"
Unit::Unit(int id, std::pair<unsigned int, unsigned int> position,
           UnitsEnum unitType, TeamEnum team)
        : id(id), position(position), unitType(unitType),
          rotation(RotationsEnum::r090), team(team),
          selected(false), actionType(ActionsEnum::STAND),
          owner(TEAM_NEUTRAL)
{
    /* units initial rotation is facing 'to the player'; action: standing. */
    //todo check what else should be initialized
}

int Unit::get_ID() {
    return id;
}

void Unit::update_position(int x, int y) {
    std::pair<int, int> new_pos(x, y);

    prev_position = position;
    position = new_pos;
    update_rotation();
}

void Unit::update_rotation() {
    auto prev_x = prev_position.first;
    auto prev_y = prev_position.second;
    auto x = position.first;
    auto y = position.second;

    if (x == prev_x and y < prev_y) {
        rotation = RotationsEnum::r090;
    } else if (x > prev_x and y < prev_y) {
        rotation = RotationsEnum::r045;
    } else if (x > prev_x and y == prev_y) {
        rotation = RotationsEnum::r000;
    } else if (x > prev_x and y > prev_y) {
        rotation = RotationsEnum::r315;
    } else if (x == prev_x and y > prev_y) {
        rotation = RotationsEnum::r270;
    } else if (x < prev_x and y > prev_y) {
        rotation = RotationsEnum::r225;
    } else if (x < prev_x, y == prev_y) {
        rotation = RotationsEnum::r180;
    } else if (x < prev_x and y < prev_y) {
        rotation = RotationsEnum::r135;
    }
}

void Unit::update_state(ActionsEnum state) {
    this->actionType = state;
}

void
Unit::markAsSelectedInRange(bool &unitsSelected, gdouble xStartCoordinate,
                            gdouble yStartCoordinate,
                            gdouble xFinishCoordinate,
                            gdouble yFinishCoordinate) {
    if (position.first >= xStartCoordinate - IMG_SIZE_IN_PX &&
        position.first <= xFinishCoordinate + IMG_SIZE_IN_PX &&
        position.second >= yStartCoordinate - IMG_SIZE_IN_PX &&
        position.second <= yFinishCoordinate + IMG_SIZE_IN_PX) {
        selected = true;
        unitsSelected = true;
    }
}

bool Unit::isShooting() {
    return shooting;
}

TeamEnum Unit::getTeam() {
    return team;
}

RotationsEnum Unit::getRotation() {
    return rotation;
}

unsigned int Unit::getXCoordinate() {
    return position.first;
}

unsigned int Unit::getYCoordinate() {
    return position.second;
}

UnitsEnum Unit::getType() {
    return unitType;
}

ActionsEnum Unit::getAction() {
    return actionType;
}

bool Unit::is_selected() {
    return selected;
}

void Unit::unselect() {
    selected = false;
}

Unit::Unit() {
}

void Unit::update_owner(const std::string &owner) {
    this->owner = owner;
}

std::string Unit::get_owner() {
    return owner;
}

int Unit::get_max_hp() {
    return totalLife;
}

int Unit::get_hp() {
    return lifeLeft;
}

void Unit::update_unit_name(const std::string &name) {
    unit_name = name;
}

std::string Unit::get_unit_name() {
    return unit_name;
}
