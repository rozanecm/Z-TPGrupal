//
// Created by rodian on 17/05/17.
//

#include "size.h"

Size::Size(int x, int y, int width, int lenght) : x(x), y(y),
                                               width(width), lenght(lenght){}

int Size::getXPosition() const {
    return x;
}

int Size::getYPosition() const {
    return y;
}

int Size::getWidth() const {
    return width;
}

int Size::getLength() const {
    return lenght;
}

bool Size::isThereACollision(Size &other) {
    int x_max, x_min, y_max, y_min;
    this->calculateMaxAndMinForX(x_max, x_min);
    this->calculateMaxAndMinForY(y_max, y_min);

   return (other.areYouOnThisPoint(x_max, y_max) ||
           other.areYouOnThisPoint(x_max, y_min) ||
           other.areYouOnThisPoint(x_min, y_max) ||
           other.areYouOnThisPoint(x_min, y_min));
}

bool Size::areYouOnThisPoint(int x_pos, int y_pos) {
    int x_max, x_min, y_max, y_min;
    this->calculateMaxAndMinForX(x_max, x_min);
    this->calculateMaxAndMinForY(y_max, y_min);

    return ((x_pos <= x_max) && (x_pos >= x_min) &&
            (y_pos <= y_max) && (y_pos >= y_min));
}

void Size::calculateMaxAndMinForX(int &max, int &min) {
    max = x + (width/2);
    min = x - (width/2);
}

void Size::calculateMaxAndMinForY(int &max, int &min) {
    max = y + (lenght/2);
    min = y - (lenght/2);
}

void Size::moveTo(int x, int y) {
    this->x = x;
    this->y = y;
}

Size::~Size() {}
