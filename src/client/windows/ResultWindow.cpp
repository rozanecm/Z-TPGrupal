#include "ResultWindow.h"

ResultWindow::ResultWindow(BaseObjectType *cobject,
                           const Glib::RefPtr<Gtk::Builder> &builder) :
        Gtk::Window(cobject)
{

    builder->get_widget("BackToMenuButton", menu);
    builder->get_widget("CloseGameButton", close);
    builder->get_widget("WinnerLabel", winner);
    builder->get_widget("LoserLabel", loser);

    menu->signal_clicked().connect(sigc::mem_fun(*this,
                                                 &ResultWindow::menu_click));
    close->signal_clicked().connect(sigc::mem_fun(*this,
                                                  &ResultWindow::close_click));
}

void ResultWindow::display_lose_screen() {
    winner->hide();
}

void ResultWindow::display_win_screen() {
    loser->hide();
}

void ResultWindow::menu_click() {
    back_to_menu = true;
    this->hide();
}

void ResultWindow::close_click() {
    back_to_menu = false;
    this->hide();
}

bool ResultWindow::go_back_to_menu() {
    return back_to_menu;
}

void ResultWindow::on_show() {
    back_to_menu = false;
    Gtk::Widget::on_show();
}


