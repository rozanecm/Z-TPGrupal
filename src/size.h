//
// Created by rodian on 17/05/17.
//

#ifndef Z_TPGRUPAL_SIZE_H
#define Z_TPGRUPAL_SIZE_H

//Class Size to represent the space on the map that an Occupant is standing on
class Size {
private:
    // (x,y) are the coordinates of the center o the object
    // all Occupants on game are Four-sided
    int x, y, width, lenght;

public:
    // width and lenght must be even numbers
    Size(int x, int y, int width, int lenght);

    int getXPosition() const;

    int getYPosition() const;

    int getWidth() const;

    int getLength() const;

    void moveTo(int x, int y);

    bool isThereACollision(Size& other);

    bool areYouOnThisPoint(int x, int y);

    ~Size();

private:
    void calculateMaxAndMinForX(int& max, int& min);

    void calculateMaxAndMinForY(int& max, int& min);
};


#endif //Z_TPGRUPAL_SIZE_H
