#include <iostream>
#include "LobbyWindow.h"
#include "../ServerMessenger.h"

LobbyWindow::LobbyWindow(BaseObjectType *cobject,
                         const Glib::RefPtr<Gtk::Builder> &builder) :
    Gtk::Window(cobject)
{

    builder->get_widget("PlayerStatus1", player1);
    builder->get_widget("PlayerStatus2", player2);
    builder->get_widget("PlayerStatus3", player3);
    builder->get_widget("PlayerStatus4", player4);
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

