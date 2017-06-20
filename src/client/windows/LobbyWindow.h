#ifndef Z_TPGRUPAL_LOBBYWINDOW_H
#define Z_TPGRUPAL_LOBBYWINDOW_H


#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/builder.h>

class LobbyWindow : public Gtk::Window {
    Gtk::Button* start;
    Gtk::Label* player1;
    Gtk::Label* player2;
    Gtk::Label* player3;
    Gtk::Label* player4;

public:
    LobbyWindow(BaseObjectType *cobject,
                const Glib::RefPtr<Gtk::Builder> &builder);

private:
    void on_click();
};


#endif //Z_TPGRUPAL_LOBBYWINDOW_H
