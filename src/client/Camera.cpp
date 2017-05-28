#include "Camera.h"

#define NUMBER_OF_TILES_TO_SHOW 10

Camera::Camera(unsigned int tileSize, unsigned int mapWidth,
               unsigned int mapHeight) :
        tileSize(tileSize),
        minXCoordinate(NUMBER_OF_TILES_TO_SHOW*tileSize/2),
        minYCoordinate(NUMBER_OF_TILES_TO_SHOW*tileSize/2),
        maxXCoordinate(mapWidth*tileSize-NUMBER_OF_TILES_TO_SHOW*tileSize/2),
        maxYCoordinate(mapHeight*tileSize-NUMBER_OF_TILES_TO_SHOW*tileSize/2){
}

std::pair<unsigned int, unsigned int> Camera::getPosition() {
    return position;
}

void Camera::setMapWidth(unsigned int width) {
    mapWidth = width;
    maxXCoordinate = mapWidth*tileSize-NUMBER_OF_TILES_TO_SHOW*tileSize/2;
}

void Camera::setMapHeight(unsigned int height) {
    mapHeight = height;
    maxYCoordinate = mapHeight*tileSize-NUMBER_OF_TILES_TO_SHOW*tileSize/2;
}
