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
    builder->get_widget("ReadyButton", ready);
    builder->get_widget("MapsLabel", maps_label);
    builder->get_widget("MapsEntry", maps_entry);
    default_label = players[0]->get_text();
    start->signal_clicked().connect(sigc::mem_fun(*this,
                                                  &LobbyWindow::click_start));

    ready->signal_clicked().connect(sigc::mem_fun(*this,
                                                  &LobbyWindow::click_ready));
}

void LobbyWindow::click_start() {
    std::string map = maps_entry->get_text();
    m->send("startgame-" + map);
}

void LobbyWindow::set_messenger(ServerMessenger &m) {
    this->m = &m;
}

void LobbyWindow::update_player_name(int at, const std::string &name) {
    if (at < 4) {
        players[at]->set_text(name);
    }
}

std::vector<std::string> LobbyWindow::get_player_names() {
    std::vector<std::string> names;
    for (Gtk::Label* player : players) {
        std::string name = player->get_text();
        if (name == default_label) {
            name = "";
        }
        names.push_back(name);
    }
    return names;
}


void LobbyWindow::click_ready() {
    if (ready->get_label() == "Ready") { // already pressed
        m->send("ready");
        ready->set_label("Unready");
    } else if(ready->get_label() == "Unready") {
        m->send("unready");
        ready->set_label("Ready");
    }
}

void LobbyWindow::start_game() {
    started = true;
    this->hide();
}

bool LobbyWindow::game_started() {
    return started;
}

void LobbyWindow::update_maps(const std::string &maps) {
    maps_label->set_text(maps);
}

void LobbyWindow::on_show() {
    started = false;
    Gtk::Widget::on_show();
}

