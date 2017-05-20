#include <gtkmm/builder.h>
#include <gdkmm.h>
#include <iostream>
#include "GameArea.h"

GameArea::GameArea() : flagCounter(0){
    /* load blue flag imgs */
    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/other/flag_blue_0.png"));

    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/other/flag_blue_1.png"));

    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/other/flag_blue_2.png"));

    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/other/flag_blue_3.png"));

    /* load some img */
    someImg = Gdk::Pixbuf::create_from_file("res/portraits/sample.png");
}

GameArea::~GameArea() { }

bool GameArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    displaySomeStaticImg(cr, 0, 0);
    drawFlagAnimation(cr, 500, 500);
    return true;
}

void GameArea::drawFlagAnimation(const Cairo::RefPtr<Cairo::Context> &cr,
                                 int xCoordinate, int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr, blueFlagVector.at(flagCounter), xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate, blueFlagVector.at(flagCounter)->get_width(),
                  blueFlagVector.at(flagCounter)->get_height());
    cr->fill();
    if (flagCounter == blueFlagVector.size()-1){
        flagCounter = 0;
    }else{
        flagCounter++;
    }
    cr->restore();
}

void
GameArea::displaySomeStaticImg(const Cairo::RefPtr<Cairo::Context> &cr,
                               int xCoordinate,
                               int yCoordinate) {
    cr->save();
    Gdk::Cairo::set_source_pixbuf(cr, someImg, xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate,
                  someImg->get_width(), someImg->get_height());
    cr->fill();
    cr->restore();
}

GameArea::GameArea(BaseObjectType *cobject,
                   const Glib::RefPtr<Gtk::Builder> &builder) :
    Gtk::DrawingArea(cobject),
    flagCounter(0)
{
    std::cout << "Llego acá? " << std::endl;
    /* load blue flag imgs */
    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/other/flag_blue_0.png"));

    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/other/flag_blue_1.png"));

    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/other/flag_blue_2.png"));

    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/other/flag_blue_3.png"));

    /* load some img */
    someImg = Gdk::Pixbuf::create_from_file("res/portraits/sample.png");
}

