#ifndef Z_TPGRUPAL_BUILDINGSMONITOR_H
#define Z_TPGRUPAL_BUILDINGSMONITOR_H


#include <mutex>
#include "Building.h"
#include <vector>

class BuildingsMonitor {
private:
    std::vector<Building> &buildings;

    std::mutex m;
public:
    explicit BuildingsMonitor(std::vector<Building> &buildings);

    void markAsSelectedInRange(bool &buildingSelected,
                                   gdouble xStartCoordinate,
                                   gdouble yStartCoordinate,
                                   gdouble xFinishCoordinate,
                                   gdouble yFinishCoordinate);

    void addBuilding(Building &b);

    std::vector<Building>
    getBuildingsToDraw(unsigned int minX, unsigned int maxX, unsigned int minY,
                       unsigned int maxY);

    std::vector<Building> get_selected();
    void wipe_selected();
    Building get_building(int id);
};


#endif //Z_TPGRUPAL_BUILDINGSMONITOR_H
