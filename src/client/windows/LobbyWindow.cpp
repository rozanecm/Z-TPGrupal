#include <iostream>
#include "LobbyWindow.h"
#include "../ServerMessenger.h"

LobbyWindow::LobbyWindow(BaseObjectType *cobject,
                         const Glib::RefPtr<Gtk::Builder> &builder) :
    Gtk::Window(cobject)
{

    builder->get_widget("PlayerStatus1", players[0]);
    builder->get_widget("PlayerStatus2", players[1]);
    builder->get_widget("PlayerStatus3", players[2]);
    builder->get_widget("PlayerStatus4", players[3]);
    builder->get_widget("StartGame", start);

    start->signal_clicked().connect(sigc::mem_fun(*this,
                                                    &LobbyWindow::on_click));
}

void LobbyWindow::on_click() {
    m->send("startgame");
}

void LobbyWindow::set_messenger(ServerMessenger &m) {
    this->m = &m;
}

void LobbyWindow::update_player_name(int at, const std::string &name) {
    if (at < 4) {
        players[at]->set_text(name);
    }
}

