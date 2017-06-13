//
// Created by rodian on 10/06/17.
//

#ifndef Z_TPGRUPAL_TERRITORY_H
#define Z_TPGRUPAL_TERRITORY_H


#include <vector>
#include "factory.h"
#include "teamable.h"

class Territory: public Teamable {
private:
    std::map<int,Factory>& factories;
    Position flag_position;
    Size territory_size;

public:
    Territory(std::map<int,Factory>& factories, std::string team,
              Position flag_position, Size territory_size);

    void grabFlag(std::string& new_team);

    int getTechLevel();

    void changeFactoriesTechLevel(int tech_level);

    Position getFlagPOsition();

    void build();

    std::map<int,Factory>& getFactories();
};


#endif //Z_TPGRUPAL_TERRITORY_H
