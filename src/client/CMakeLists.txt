cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 11)
project(Z_TPGrupal)

set(LIBS_DIR libs)
set(PUGI ${LIBS_DIR}/xml)

find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0)
pkg_check_modules(SDL2 sdl2)
pkg_check_modules(SDL2_mixer SDL2_mixer)

link_directories(
        ${SDL2_mixer_LIBRARY_DIRS}
        ${GTKMM_LIBRARY_DIRS}
        ../common)

include_directories(
        ${PUGI}
        ${GTKMM_INCLUDE_DIRS}
        ${SDL2_LIBRARY_DIRS}
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_mixer_INCLUDE_DIRS}
        ../common)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(SOURCE_FILES
        ../../libs/xml/pugixml.hpp
        ../../libs/xml/pugixml.cpp
        Armament.cpp
        Armament.h
        Building.cpp
        Building.h
        BuildingPanel.cpp
        BuildingPanel.h
        BuildingsMonitor.cpp
        BuildingsMonitor.h
        Camera.cpp
        Camera.h
        Cell.cpp
        Cell.h
        ClientThread.cpp
        ClientThread.h
        GameArea.cpp
        GameArea.h
        GameBuilder.cpp
        GameBuilder.h
        GameWindow.cpp
        GameWindow.h
        GraphicsThread.cpp
        GraphicsThread.h
        main.cpp
        Map.cpp
        Map.h
        MapMonitor.cpp
        MapMonitor.h
        Player.cpp
        Player.h
        PlayersMonitor.cpp
        PlayersMonitor.h
        Unit.cpp
        Unit.h ServerMessenger.cpp ServerMessenger.h
        commands/Command.h
        commands/AddUnit.cpp
        commands/AddUnit.h commands/RemoveUnit.cpp commands/RemoveUnit.h commands/UpdatePosition.cpp commands/UpdatePosition.h FlagEnum.h TeamEnum.h ActionsEnum.h UnitsEnum.h BuildingsEnum.h commands/LoadMap.cpp commands/LoadMap.h windows/InitialWindow.cpp windows/InitialWindow.h)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_compile_options(${SDL2_mixer_LDFLAGS} ${SDL2_mixer_CFLAGS})
add_executable(Z_Client ${SOURCE_FILES})

target_link_libraries(Z_Client
        ${GTKMM_LIBRARIES}
        ${SDL_LIBRARY}
        ${SDL2_mixer_LIBRARIES}
        Threads::Threads
        common)