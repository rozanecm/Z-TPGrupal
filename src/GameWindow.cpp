#include <iostream>
#include "GameWindow.h"

#define SCREENWIDTH 1920
#define SCREENHEIGHT 1080

GameWindow::GameWindow() {
    /* set window size */
//    set_size_request(SCREENWIDTH, SCREENHEIGHT);
    fullscreen();

    /* set window title */
    set_title("Z");

    /* add main box to window */
    add(mainGrid);

    gameArea.set_size_request(SCREENWIDTH * 6 / 7, SCREENHEIGHT);
    mainGrid.add(gameArea);
    show_all_children();
}

GameWindow::~GameWindow() {

}
