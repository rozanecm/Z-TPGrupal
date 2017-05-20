#include <gtkmm/builder.h>
#include <gdkmm.h>
#include "GameArea.h"

GameArea::GameArea() : flagCounter(0){
    /* load blue flag imgs */
    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/other/flag_blue_0.png")->scale_simple(16*3, 16*3, Gdk::INTERP_BILINEAR));
    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/other/flag_blue_1.png")->scale_simple(16*3, 16*3, Gdk::INTERP_BILINEAR));
    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/other/flag_blue_2.png")->scale_simple(16*3, 16*3, Gdk::INTERP_BILINEAR));
    blueFlagVector.emplace_back(Gdk::Pixbuf::create_from_file(
            "res/assets/other/flag_blue_3.png")->scale_simple(16*3, 16*3, Gdk::INTERP_BILINEAR));

    /* load some img */
    auto someImg = Gdk::Pixbuf::create_from_file("res/img.png");
    someImg->scale_simple(someImg->get_width()*5, someImg->get_height()*5,
                          Gdk::INTERP_BILINEAR);
}

GameArea::~GameArea() { }

bool GameArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
//    displaySomeStaticImg(cr);
    drawFlagAnimation(cr, 0, 0);
    return true;
}

void GameArea::drawFlagAnimation(const Cairo::RefPtr<Cairo::Context> &cr,
                                 int xCoordinate, int yCoordinate) {
    Gdk::Cairo::set_source_pixbuf(cr, blueFlagVector.at(flagCounter), xCoordinate, yCoordinate);
    cr->rectangle(xCoordinate, yCoordinate, blueFlagVector.at(flagCounter)->get_width(),
                  blueFlagVector.at(flagCounter)->get_height());
    cr->fill();
    if (flagCounter == blueFlagVector.size()-1){
        flagCounter = 0;
    }else{
        flagCounter++;
    }
}

void
GameArea::displaySomeStaticImg(const Cairo::RefPtr<Cairo::Context> &cr) {
    Gdk::Cairo::set_source_pixbuf(cr, blueFlagVector.at(flagCounter), 100, 80);
    cr->rectangle(110, 90, blueFlagVector.at(flagCounter)->get_width(),
                  blueFlagVector.at(flagCounter)->get_height());
    cr->fill();
}
