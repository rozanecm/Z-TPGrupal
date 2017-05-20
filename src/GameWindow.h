#ifndef Z_TPGRUPAL_GAMEWINDOW_H
#define Z_TPGRUPAL_GAMEWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/grid.h>
#include <gtkmm/button.h>
#include "GameArea.h"

class GameWindow : public Gtk::Window {
private:
    GameArea gameArea;

public:
    GameWindow();

    virtual ~GameWindow();

protected:
    Gtk::Grid mainGrid;

    bool onTimeout();

};


#endif //Z_TPGRUPAL_GAMEWINDOW_H
