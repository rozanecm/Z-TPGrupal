#ifndef Z_TPGRUPAL_MENUWINDOW_H
#define Z_TPGRUPAL_MENUWINDOW_H


#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include <gtkmm/listbox.h>
#include <gtkmm/button.h>

class MenuWindow : public Gtk::Window {
    Gtk::ListBox* available_lobbies;
    Gtk::Button* join_button;
    Gtk::Button* create_button;

public:
    MenuWindow(BaseObjectType *cobject,
               const Glib::RefPtr<Gtk::Builder> &builder);

    void join_lobby();
};


#endif //Z_TPGRUPAL_MENUWINDOW_H
