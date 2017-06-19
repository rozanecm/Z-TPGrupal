//
// Created by rodian on 17/05/17.
//

#ifndef Z_TPGRUPAL_SIZE_H
#define Z_TPGRUPAL_SIZE_H

#include "position.h"

//Class Size to represent the space on the map that an Occupant is standing on
class Size {
private:
    // (x,y) are the coordinates of the top-left corner of the object
    // all Occupants on game are Four-sided
    Position position;
    int width, height;

public:
    // width and lenght must be even numbers
    Size(int x, int y, int width, int height);

    // Returns the x position on the map
    Position getPosition() const;

    // Returns the width of the Object of this size
    int getWidth() const;

    // Returns the lenght of the Object of this size
    int getHeight() const;

    // Changes the position of the Object of this size to a new (x,y)
    void moveTo(int x, int y);

    // Returns True is this Size collisions with other.
    // Meant to check collision between different objects.
    // To fully be sure there is no collison, use this method on the size of
    // the other object as well
    bool isThereACollision(Size& other);

    // If other is partly or completely outside returns true.
    // If other is all inside this returns false.
    bool areYouHalfOutSide(Size& other);

    // Returns true if the position (x,y) received is inside this Size
    bool areYouOnThisPoint(int x, int y);

    ~Size();

    // Writes on max and min the maximum value on 'x' coordinate that this size
    // has and the minimum value on min
    void calculateMaxAndMinForX(int& max, int& min);

    // Writes on max and min the maximum value on 'y' coordinate that this size
    // has and the minimum value on min
    void calculateMaxAndMinForY(int& max, int& min);
};


#endif //Z_TPGRUPAL_SIZE_H
