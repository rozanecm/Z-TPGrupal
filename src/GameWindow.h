#ifndef Z_TPGRUPAL_GAMEWINDOW_H
#define Z_TPGRUPAL_GAMEWINDOW_H

#include <gtkmm.h>

class GameWindow {
    Gtk::Window* window;
    Gtk::Image* portrait;
public:
    GameWindow();
    ~GameWindow();

    Gtk::Window* get_window();
private:
    void update_portrait(const std::string& img_path) const;
};


#endif //Z_TPGRUPAL_GAMEWINDOW_H
