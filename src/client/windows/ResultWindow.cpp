#include "ResultWindow.h"

ResultWindow::ResultWindow(BaseObjectType *cobject,
                           const Glib::RefPtr<Gtk::Builder> &builder) :
        Gtk::Window(cobject) {

    builder->get_widget("BackToMenuButton", menu);
    builder->get_widget("CloseGameButton", close);
    builder->get_widget("WinnerLabel", winner);
    builder->get_widget("LoserLabel", loser);
}
