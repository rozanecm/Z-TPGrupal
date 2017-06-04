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

    void markAsSelectedInRange(gdouble xStartCoordinate,
                               gdouble yStartCoordinate,
                               gdouble xFinishCoordinate,
                               gdouble yFinishCoordinate);
};


#endif //Z_TPGRUPAL_BUILDINGSMONITOR_H
