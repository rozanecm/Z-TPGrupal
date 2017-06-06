#include "Camera.h"
#include <utility>

Camera::Camera(unsigned int tileSize, unsigned int mapWidth,
               unsigned int mapHeight,
               unsigned int numberOfTilesToShow) :
        tileSize(tileSize),
        numberOfTilesToShow(numberOfTilesToShow),
        minXCoordinate(numberOfTilesToShow*tileSize/2),
        minYCoordinate(numberOfTilesToShow*tileSize/2),
        maxXCoordinate(mapWidth*tileSize-numberOfTilesToShow*tileSize/2),
        maxYCoordinate(mapHeight*tileSize-numberOfTilesToShow*tileSize/2),
        position(numberOfTilesToShow*tileSize/2,
                 numberOfTilesToShow*tileSize/2){
}

/**
 *
 * @return camera position is given in pixels.
 */
std::pair<unsigned int, unsigned int> Camera::getPosition() {
    return position;
}

void Camera::setMapWidth(unsigned int width) {
    mapWidth = width;
    maxXCoordinate = mapWidth*tileSize-numberOfTilesToShow*tileSize/2;
}

void Camera::setMapHeight(unsigned int height) {
    mapHeight = height;
    maxYCoordinate = mapHeight*tileSize-numberOfTilesToShow*tileSize/2;
}

void Camera::moveUp() {
    if (position.second != minYCoordinate){
        position.second -= tileSize;
    }
}

void Camera::moveDown() {
    if (position.second != maxYCoordinate){
        position.second += tileSize;
    }
}

void Camera::moveRight() {
    if (position.first != maxXCoordinate){
        position.first += tileSize;
    }
}

void Camera::moveLeft() {
    if (position.first != minXCoordinate){
        position.first -= tileSize;
    }
}

std::pair<unsigned int, unsigned int> Camera::cameraOffset() {
    return std::pair<unsigned int, unsigned int>
            (position.first - numberOfTilesToShow * tileSize / 2,
             position.second - numberOfTilesToShow * tileSize / 2);
}
