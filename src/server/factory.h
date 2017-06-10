//
// Created by rodian on 22/05/17.
//

#ifndef Z_TPGRUPAL_FACTORY_H
#define Z_TPGRUPAL_FACTORY_H
#include "unitMold.h"

class Factory: public Occupant {
    bool running;
    int tech_level,time_counter;
    std::vector<UnitMold> units;
    std::vector<UnitMold>::iterator it;
    std::vector<Unit> new_units;
    Map& map;
    std::map<std::string, Weapon> weapons;


public:
    Factory(int id, int life, std::string type, Size position,
                std::vector<UnitMold> units, Map &map,
                std::map<std::string, Weapon> &weapons);

    // starts the creation of the selected unit
    void build();

    void startBuilding(std::string& player_id);

    // Stops the creation of units
    void stopBuilding(std::string &player_id);

    // Returns the creational time of the selected Unit
    int getSelectedUnitTime();

    // Returns the type of the unit that now is selected
    std::string nextUnit();

    Occupant* destroyFactory();

    void changeTechLevel(int tech_level);

    int getCreationSpeed();

    bool haveNewUnits();

    std::vector<Unit> getUnits();
};


#endif //Z_TPGRUPAL_FACTORY_H
