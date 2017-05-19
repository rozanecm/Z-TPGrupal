//
// Created by rodian on 18/05/17.
//

#ifndef Z_TPGRUPAL_POSITION_H
#define Z_TPGRUPAL_POSITION_H

// Position class to keep the coordinates of the map
class Position {
private:
    int x, y;
public:
    Position(int x, int y);

    // Returns the x coordinate
    int getX() const;

    // Returns the y coordinate
    int getY() const;

    // Changes the position to the (x,y) received
    void moveTo(int x, int y);
};


#endif //Z_TPGRUPAL_POSITION_H
