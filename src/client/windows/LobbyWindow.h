#ifndef Z_TPGRUPAL_LOBBYWINDOW_H
#define Z_TPGRUPAL_LOBBYWINDOW_H


#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/builder.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/entry.h>

#define PLAYERS_AMT 4
#include "../ServerMessenger.h"

class LobbyWindow : public Gtk::Window {

    Gtk::Button *start;
    Gtk::Button *ready;
    Gtk::Label *players[PLAYERS_AMT];
    ServerMessenger *m;
    Gtk::Label* maps_label;
    Gtk::Entry* maps_entry;
    std::string default_label;

    bool started = false;
public:
    LobbyWindow(BaseObjectType *cobject,
                const Glib::RefPtr<Gtk::Builder> &builder);

    void set_messenger(ServerMessenger& m);
    void update_player_name(int at, const std::string& name);

    std::vector<std::string> get_player_names();
    void start_game();
    bool game_started();

    void update_maps(const std::string& maps);
private:
    void click_start();
    void click_ready();
};


#endif //Z_TPGRUPAL_LOBBYWINDOW_H
