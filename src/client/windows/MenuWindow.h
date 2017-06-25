#ifndef Z_TPGRUPAL_MENUWINDOW_H
#define Z_TPGRUPAL_MENUWINDOW_H


#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include <gtkmm/listbox.h>
#include <gtkmm/button.h>
#include <gtkmm/listviewtext.h>
#include <gtkmm/textview.h>
#include "../ServerMessenger.h"

class MenuWindow : public Gtk::Window {
    Gtk::Label* available_lobbies;
    Gtk::Button* join_button;
    Gtk::Button* create_button;
    Gtk::Entry* lobby_entry;
    ServerMessenger* messenger;

    bool joined_successfully;
public:
    MenuWindow(BaseObjectType *cobject,
               const Glib::RefPtr<Gtk::Builder> &builder);

    void join_click();

    void load_messenger(ServerMessenger* messenger);

    void create_click();
    void update_lobbies(const std::vector<std::string>& lobbies);

    void on_show();

    void join_lobby();
    bool joined_lobby();
};


#endif //Z_TPGRUPAL_MENUWINDOW_H
