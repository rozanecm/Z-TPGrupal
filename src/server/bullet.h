//
// Created by rodian on 22/05/17.
//

#ifndef Z_TPGRUPAL_BULLET_H
#define Z_TPGRUPAL_BULLET_H

#include <string>
#include <vector>
#include "size.h"
#include "Occupant.h"

class Bullet {
private:
    std::string type;
    int damage, w_speed;
    Size w_size;
    bool hit;
    Occupant& target;
    std::vector<Position>* road;

public:
    Bullet(std::string type, int damage, int w_speed, Size w_size,
           Occupant target);

    // Pursues the Target in straight line on each TIC til hits
    void shotTarget(Occupant& target);

    void calculateRoadToTarget();

    void move();

    bool didHit();

    Position calculateNextPosition(double a, double b, int x);

    Position calculateNextInvertPosition(double a,double b,int y);

    Size getSize();
};


#endif //Z_TPGRUPAL_BULLET_H
