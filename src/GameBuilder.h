#ifndef Z_TPGRUPAL_GAMEWINDOW_H
#define Z_TPGRUPAL_GAMEWINDOW_H

#include <gtkmm.h>

class GameBuilder {
    Glib::RefPtr<Gtk::Builder> refBuilder;
    Gtk::Window* window;
    Gtk::Image* portrait;
    Gtk::Box* panel;
    Gtk::Box* unit_panel;
    Gtk::Box* building_panel;
    Gtk::Box* group_panel;
    Gtk::Button* button;
public:
    GameBuilder();
    ~GameBuilder();

    Gtk::Window* get_window();
    bool change_view_to_unit();
    bool change_view_to_building();
    bool change_view_to_unit_group();
private:
    void update_portrait(const std::string& img_path) const;
};


#endif //Z_TPGRUPAL_GAMEWINDOW_H
