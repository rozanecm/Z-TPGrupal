//
// Created by rodian on 22/05/17.
//

#ifndef Z_TPGRUPAL_UNITMOLD_H
#define Z_TPGRUPAL_UNITMOLD_H


#include "unit.h"

class UnitMold {
private:
    int tec_level, life, range, width, height, unit_speed, fire_rate,
            creation_time;
    std::string type;

public:
    UnitMold(int tec_level, int life, int range, int width, int height,
           int unit_speed, int fire_rate, int creation_time, std::string type);

    Unit* createUnit(int x, int y, Map& map);

    // Returns the technology level needed to create Tough unit
    int getTechnologyLevel() const;

    // Returns creation time in seconds
    int getCreationTime();

    // Returns the type of unit that this mold creates
    std::string getTypeOfUnit() const;
};


#endif //Z_TPGRUPAL_UNITMOLD_H
