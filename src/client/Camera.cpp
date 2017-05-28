#include "Camera.h"

Camera::Camera(unsigned int tileSize, unsigned int mapWidth, unsigned int mapHeight,
               unsigned int numberOfTilesToShow) :
        tileSize(tileSize), numberOfTilesToShow(numberOfTilesToShow),
        minXCoordinate(numberOfTilesToShow*tileSize/2),
        minYCoordinate(numberOfTilesToShow*tileSize/2),
        maxXCoordinate(mapWidth*tileSize-numberOfTilesToShow*tileSize/2),
        maxYCoordinate(mapHeight*tileSize-numberOfTilesToShow*tileSize/2){
}

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
