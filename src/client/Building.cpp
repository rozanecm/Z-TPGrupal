#include "Building.h"

void Building::markAsSelectedInRange(bool &buildingSelected,
                                     gdouble xStartCoordinate,
                                     gdouble yStartCoordinate,
                                     gdouble xFinishCoordinate,
                                     gdouble yFinishCoordinate) {
    if (position.first >= xStartCoordinate &&
        position.first <= xFinishCoordinate &&
        position.second >= yStartCoordinate &&
        position.second <= yFinishCoordinate) {
        selected = true;
        buildingSelected = true;
    }
}

Building::Building(BuildingsEnum type, int x, int y, int id, TeamEnum team) :
        buildingType(type),
        position(x, y),
        id(id),
        selected(false),
        team (team) {
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
