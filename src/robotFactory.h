//
// Created by rodian on 22/05/17.
//

#ifndef Z_TPGRUPAL_ROBOTFACTORY_H
#define Z_TPGRUPAL_ROBOTFACTORY_H


#include "unitMold.h"
#include "factory.h"

class RobotFactory: public Factory {
private:
    std::vector<UnitMold> units;
    std::vector<UnitMold>::iterator it;

public:
    RobotFactory(int life, std::string type, Size position,
                                    std::vector<UnitMold> units);

    // starts the creation of the selected unit
    void build();

    // Stops the creation of units
    void stopBuilding();

    // Returns the creational time of the selected Unit
    int getSelectedUnitTime();

    // Returns the type of the unit that now is selected
    std::string nextUnit();

    Occupant* destroyFactory();
};


#endif //Z_TPGRUPAL_ROBOTFACTORY_H
