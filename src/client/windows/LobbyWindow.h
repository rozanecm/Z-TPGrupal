#ifndef Z_TPGRUPAL_LOBBYWINDOW_H
#define Z_TPGRUPAL_LOBBYWINDOW_H


#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/builder.h>

#define PLAYERS_AMT 4
#include "../ServerMessenger.h"
class LobbyWindow : public Gtk::Window {
    Gtk::Button* start;
    Gtk::Label* players[PLAYERS_AMT];
    ServerMessenger* m;
    std::string default_label;
public:
    LobbyWindow(BaseObjectType *cobject,
                const Glib::RefPtr<Gtk::Builder> &builder);

    void set_messenger(ServerMessenger& m);
    void update_player_name(int at, const std::string& name);

    std::vector<std::string> get_player_names();
private:
    void on_click();
};


#endif //Z_TPGRUPAL_LOBBYWINDOW_H
