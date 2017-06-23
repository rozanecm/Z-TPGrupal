#include "Building.h"

#define LENIENCY 8
void Building::markAsSelectedInRange(bool &buildingSelected,
                                     gdouble xStartCoordinate,
                                     gdouble yStartCoordinate,
                                     gdouble xFinishCoordinate,
                                     gdouble yFinishCoordinate) {
    int x = (int) xFinishCoordinate;
    int y = (int) yFinishCoordinate;
    int x_abs = abs(position.first - x);
    int y_abs = abs(position.second - y);

    if (x_abs <= LENIENCY && y_abs <= LENIENCY) {
        selected = true;
        buildingSelected = true;
    }
}

Building::Building(BuildingsEnum type, int x, int y, int id, TeamEnum team,
                   const std::string& owner, unsigned int hp) :
        buildingType(type),
        position(x, y),
        id(id),
        selected(false),
        team (team),
        owner(owner),
        hp(hp),
        max_hp(hp)

{
}

unsigned int Building::getXCoordinate() {
    return position.first;
}

unsigned int Building::getYCoordinate() {
    return position.second;
}

BuildingsEnum Building::getBuildingType() {
    return buildingType;
}

TeamEnum Building::getTeam() {
    return team;
}

bool Building::is_selected() {
    return selected;
}

void Building::unselect() {
    selected = false;
}

Building::Building() {
    id = 0;
}

std::string Building::get_owner() {
    return owner;
}

unsigned int Building::get_hp() {
    return hp;
}

unsigned int Building::get_max_hp() {
    return max_hp;
}

int Building::get_ID() {
    return id;
}
