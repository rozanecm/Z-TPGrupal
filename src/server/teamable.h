//
// Created by rodian on 22/05/17.
//

#ifndef Z_TPGRUPAL_TEAMABLE_H
#define Z_TPGRUPAL_TEAMABLE_H

#include <iostream>
#include "size.h"

class Teamable {
protected:
    std::string team;
    Size obj_size;

public:
    Teamable(std::string team, Size obj_size);

    // Builds a Teamable with "Neutral" team
    Teamable(Size size);

    // Changes the team of the object
    void changeTeam(std::string team);

    // Returns the actual team of the object
    std::string getTeam() const;

    // Returns the central position of the object
    Position getPosition();

    // Returns true if is there a collision with the object
    bool isThereACollision(Size& size);

    Size getSize();
};


#endif //Z_TPGRUPAL_TEAMABLE_H
