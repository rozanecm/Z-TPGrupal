cmake_minimum_required(VERSION 3.6)
project(Z_TPGrupal)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        libs/xml/pugiconfig.hpp
        libs/xml/pugixml.cpp
        libs/xml/pugixml.hpp
        src/client/Armament.cpp
        src/client/Armament.h
        src/client/Building.cpp
        src/client/Building.h
        src/client/BuildingPanel.cpp
        src/client/BuildingPanel.h
        src/client/BuildingsMonitor.cpp
        src/client/BuildingsMonitor.h
        src/client/Cell.cpp
        src/client/Cell.h
        src/client/ClientThread.cpp
        src/client/ClientThread.h
        src/client/GameArea.cpp
        src/client/GameArea.h
        src/client/GameBuilder.cpp
        src/client/GameBuilder.h
        src/client/GameWindow.cpp
        src/client/GameWindow.h
        src/client/GraphicsThread.cpp
        src/client/GraphicsThread.h
        src/client/main.cpp
        src/client/Map.cpp
        src/client/Map.h
        src/client/MapMonitor.cpp
        src/client/MapMonitor.h
        src/client/Player.cpp
        src/client/Player.h
        src/client/PlayersMonitor.cpp
        src/client/PlayersMonitor.h
        src/client/Unit.cpp
        src/client/Unit.h
        src/Lock.cpp
        src/Lock.h
        src/Thread.cpp
        src/Thread.h)

add_executable(Z_TPGrupal ${SOURCE_FILES})