#ifndef Z_TPGRUPAL_GAMEAREA_H
#define Z_TPGRUPAL_GAMEAREA_H


#include <gtkmm/drawingarea.h>
#include <utility>
#include "BuildingsMonitor.h"
#include "MapMonitor.h"
#include "Camera.h"
#include "enums/TeamEnum.h"
#include "enums/ActionsEnum.h"
#include "enums/UnitsEnum.h"
#include "enums/BuildingsEnum.h"
#include "enums/RotationsEnum.h"
#include "Counter.h"
#include "UnitsMonitor.h"
#include <map>
#include <string>
#include <vector>

class GameArea : public Gtk::DrawingArea {
private:
    /* shared resources */
    UnitsMonitor *unitsMonitor;
    BuildingsMonitor *buildingsMonitor;
    MapMonitor *mapMonitor;

    Camera camera;

    /* general resources */
    std::map<std::string, Glib::RefPtr<Gdk::Pixbuf>> tiles;

    std::map<NatureEnum, Glib::RefPtr<Gdk::Pixbuf>> nature;
    Glib::RefPtr<Gdk::Pixbuf> someImg;

    /* map holding all units imgs */
    std::map<TeamEnum,
            std::map<UnitsEnum,
                    std::map<ActionsEnum,
                            std::map<RotationsEnum,
                                    std::vector<Glib::RefPtr<Gdk::Pixbuf>>>>>>
            unitsAnimations;

    std::map<RotationsEnum, std::vector<Glib::RefPtr<Gdk::Pixbuf>>> jeepTires;

    /* BUILDINGS RESOURCES */
    std::map<BuildingsEnum, std::vector<Glib::RefPtr<Gdk::Pixbuf>>> buildings;

    /* map holding all flags */
    std::map<TeamEnum, std::vector<Glib::RefPtr<Gdk::Pixbuf>>> flags;

    /* declare counter used to know which of the flag imgs
     * which compose the flag's animation should be showed. This counters are
     * updated every time on_draw() is called.
     * */
    Counter flagCounter;
    Counter standingRobotCounter;
    Counter walkingRobotCounter;
    Counter shootingRobotCounter;
    Counter jeepCounter;
    Counter tireCounter;
    Counter tankCounter;
    Counter mmlCounter;
    Counter buildingsCounter;

    bool move_cmd = false;
    std::pair<int, int> coords;

    /* DRAWING METHODS */
    void drawBaseMap(const Cairo::RefPtr<Cairo::Context> &cr);

    void drawTileAt(const Cairo::RefPtr<Cairo::Context> &cr,
                    unsigned int xTileCoordinate, unsigned int yTileCoordinate,
                    std::string terrainType);

    void drawFlagAnimation(const Cairo::RefPtr<Cairo::Context> &ptr,
                           int xGraphicCoordinate, int yGraphicCoordinate);

    void drawBuildingsInView(const Cairo::RefPtr<Cairo::Context> &cr);

    void displaySomeStaticImg(const Cairo::RefPtr<Cairo::Context> &refPtr,
                              int xCoordinate, int yCoordinate);


    /* Event handling methods */
    bool on_key_press_event(GdkEventKey *event) override;

    bool on_button_press_event(GdkEventButton *event) override;

    bool on_button_release_event(GdkEventButton *event) override;

    /* vars. for event handling */
    /* this coordinates are sytem coordinatess */
    gdouble xStartCoordinate;
    gdouble xFinishCoordinate;
    gdouble yStartCoordinate;
    gdouble yFinishCoordinate;
    bool selectionMade;

    /* unitsSelected is true if the players' units are selected. This is used
     * to manage user clicks.
     * Turns true in Unit::markAsSelectedInRange, when some unit has been
     * selected.
     * Turns to false at the end of GameArea::processSelection(), when the
     * selection has already been processed. */
    bool unitsSelected;
    bool buildingSelected;
    bool terrainSelected;

    void makeSelection();

    /* FLAG res loading */
    void loadFlagAnimations();

    /* UNIT FIRE res loading */
    void loadUnitsResources();

    void loadGruntFireAnimations();

    void loadLaserFireAnimations();

    void loadPsychoFireAnimations();

    void loadPyroFireAnimations();

    void loadSniperFireAnimations();

    void loadToughFireAnimations();

    void loadBlueGruntFireAnimations();

    void loadGreenGruntFireAnimations();

    void loadRedGruntFireAnimations();

    void loadYellowGruntFireAnimations();

    void loadBlueLaserFireAnimations();

    void loadGreenLaserFireAnimations();

    void loadRedLaserFireAnimations();

    void loadYellowLaserFireAnimations();

    void loadBluePsychoFireAnimations();

    void loadGreenPsychoFireAnimations();

    void loadRedPsychoFireAnimations();

    void loadYellowPsychoFireAnimations();

    void loadBluePyroFireAnimation();

    void loadGreenPyroFireAnimation();

    void loadRedPyroFireAnimation();

    void loadYellowPyroFireAnimation();

    void loadBlueSniperFireAnimations();

    void loadGreenSniperFireAnimations();

    void loadRedSniperFireAnimations();

    void loadYellowSniperFireAnimations();

    void loadBlueToughFireAnimations();

    void loadGreenToughFireAnimations();

    void loadRedToughFireAnimations();

    void loadYellowToughFireAnimations();

    /* UNIT WALK res loading */
    void loadBlueWalkingAnimations();

    void loadGreenWalkingAnimations();

    void loadRedWalkingAnimations();

    void loadYellowWalkingAnimations();

    /* UNIT STAND res loading */
    void loadBlueStandingAnimations();

    void loadGreenStandingAnimations();

    void loadRedStandingAnimations();

    void loadYellowStandingAnimations();

    /* BUILDING res loading */
    void loadBuildingsResources();

    /* VEHICLES res loading */
    void loadTiresAnimations();

    void loadJeepTires();

    void loadNeuterVehiclesAnimations();

    void loadNeuterJeepAnimations();

    void loadBlueVehiclesAnimations();

    void loadGreenVehiclesAnimations();

    void loadRedVehiclesAnimations();

    void loadYellowVehiclesAnimations();

    void loadBlueJeepAnimations();

    void loadBlueLightTankAnimations();

    void loadBlueMediumTankAnimations();

    void loadBlueMMLAnimations();

    void loadBlueHeavyTankAnimations();

    void loadGreenJeepAnimations();

    void loadGreenLightTankAnimations();

    void loadGreenMediumTankAnimations();

    void loadGreenMMLAnimations();

    void loadGreenHeavyTankAnimations();

    void loadRedJeepAnimations();

    void loadRedLightTankAnimations();

    void loadRedMediumTankAnimations();

    void loadRedMMLAnimations();

    void loadRedHeavyTankAnimations();

    void loadYellowJeepAnimations();

    void loadYellowLightTankAnimations();

    void loadYellowMediumTankAnimations();

    void loadYellowMMLAnimations();

    void loadYellowHeavyTankAnimations();

    void drawJeepTires(const Cairo::RefPtr<Cairo::Context> &cr,
                       unsigned int xGraphicCoordinate,
                       unsigned int yGraphicCoordinate,
                       RotationsEnum rotation);

    void drawUnit(TeamEnum team, UnitsEnum unitType, ActionsEnum actionType,
                  RotationsEnum rotation, unsigned short unitCounter,
                  const Cairo::RefPtr<Cairo::Context> &cr,
                  unsigned int xGraphicCoordinate,
                  unsigned int yGraphicCoordinate);

    void loadResources();

    /**
     * draws all units that are in camera's scope
     * @param cr receive smart pointer to cairo context
     */
    void drawUnitsInMap(const Cairo::RefPtr<Cairo::Context> &cr);

    /**
     * counters used to know whic img of each animation should be drawn are
     * updated. This method should get called once per frame, otherwise it is
     * not guaranteed that animations will be correctly drawn.
     */
    void updateCounters();

protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;

public:
    virtual ~GameArea();

    GameArea(BaseObjectType *cobject,
             const Glib::RefPtr<Gtk::Builder> &builder);

    /**
     * initialize shared resources.
     */
    void
    setResources(UnitsMonitor *playersMonitor,
                 BuildingsMonitor *buildingsMonitor,
                 MapMonitor *mapMonitor);

    void processClick();

    void initializeCounters();

    unsigned short getCounter(Unit &unit) const;

    void processUnitToDrawEnums(UnitsEnum &unitType, ActionsEnum &actionType,
                                RotationsEnum &rotation) const;

    unsigned int cameraToRealMapX(unsigned int coordinate);

    unsigned int cameraToRealMapY(unsigned int coordinate);

    std::pair<int, int> get_coords();

    unsigned int screenMapToCameraX(gdouble coordinate);

    unsigned int screenMapToCameraY(gdouble coordinate);

    void setMapData();

    void drawBuilding(BuildingsEnum buildingType, unsigned short counter,
                          TeamEnum team,
                          const Cairo::RefPtr<Cairo::Context> &cr,
                          unsigned int xGraphicCoordinate,
                          unsigned int yGraphicCoordinate);

    bool unitIsRobot(UnitsEnum unitType);

    void drawFlag(const TeamEnum &team, const Cairo::RefPtr<Cairo::Context> &cr,
                  unsigned int xGraphicCoordinate, unsigned int yGraphicCoordinate) const;

    bool unit_selected();
    bool buildings_selected();

    void drawNatureInView(const Cairo::RefPtr<Cairo::Context> &cr);

    void drawNature(NatureEnum natureType, const Cairo::RefPtr<Cairo::Context> &cr,
                    unsigned int x, unsigned int y);

    void loadMapResources();
};


#endif //Z_TPGRUPAL_GAMEAREA_H
