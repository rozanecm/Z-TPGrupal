#include <iostream>
#include "GameWindow.h"

#define SCREENWIDTH 1920
#define SCREENHEIGHT 1000

GameWindow::GameWindow() : button1("button1"){
    /* set window size */
    set_size_request(SCREENWIDTH, SCREENHEIGHT);
//    fullscreen();

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
