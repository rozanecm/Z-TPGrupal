//
// Created by rodian on 18/05/17.
//

#ifndef Z_TPGRUPAL_UNIT_H
#define Z_TPGRUPAL_UNIT_H

#include "Occupant.h"
#include "compass.h"
//#include "map.h"

class Map;

class Unit: public Occupant {
private:
    Compass compass;
    int unit_speed;
    // State of Unit can be "atk" if is attacking, "mv" if is moving, "std" if
    // is standing still
    std::string state, team;
    Size range;

public:
    Unit(int life, std::string type, int unit_speed, Size size, Size range,
                                                                Map& map);

    // Changes the unit team
    void setTeam(std::string team);

    // Indicates the Unit it's new position
    void moveToPosition(int x, int y);

    // Returns the current position of the unit
    Position getCurrentPosition() const;

    // Returns "atk" if is attacking, "mv" if is moving, "std" if
    // is standing still
    std::string getState() const;

};


#endif //Z_TPGRUPAL_UNIT_H
