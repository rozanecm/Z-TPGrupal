#include "BuildingsMonitor.h"
#include <vector>
#include <Lock.h>

BuildingsMonitor::BuildingsMonitor(std::vector<Building> &buildings) :
        buildings(buildings){
}

void BuildingsMonitor::markAsSelectedInRange(gdouble xStartCoordinate,
                                             gdouble yStartCoordinate,
                                             gdouble xFinishCoordinate,
                                             gdouble yFinishCoordinate) {
    Lock l(m);
    for (Building& building : buildings){
        /* each player has to selects its units in range */
        building.markAsSelectedInRange(xStartCoordinate, yStartCoordinate,
                                     xFinishCoordinate, yFinishCoordinate);
    }
}

void BuildingsMonitor::addBuilding(Building &b) {
    Lock l(m);
    buildings.push_back(b);
}
