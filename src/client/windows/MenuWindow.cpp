#include <gtkmm/label.h>
#include "MenuWindow.h"

MenuWindow::MenuWindow(BaseObjectType *cobject,
                       const Glib::RefPtr<Gtk::Builder> &builder) :
    Gtk::Window(cobject)
{

    builder->get_widget("AvailableLobbies", available_lobbies);
    builder->get_widget("CreateLobbyButton", create_button);
    builder->get_widget("JoinLobbyButton", join_button);
    builder->get_widget("LobbyEntry", lobby_entry);
    join_button->signal_clicked().connect(sigc::mem_fun(*this,
                                                        &MenuWindow::join_lobby));
    create_button->signal_clicked().connect(sigc::mem_fun(*this,
                                                          &MenuWindow::create_lobby));
}

void MenuWindow::join_lobby() {
    std::string lobby = lobby_entry->get_text();
    messenger->send("joinlobby-" + lobby);
    this->hide();
    messenger->send("ready");

}

void MenuWindow::load_messenger(ServerMessenger *messenger) {
    this->messenger = messenger;
}

void MenuWindow::create_lobby() {
    messenger->send("createlobby");
    this->hide();
    messenger->send("ready");
}

void MenuWindow::update_lobbies(const std::vector<std::string> &lobbies) {
    std::stringstream text;
    for (const std::string& lobby : lobbies) {
        text << lobby << std::endl;
    }
    available_lobbies->set_text(text.str());
}
