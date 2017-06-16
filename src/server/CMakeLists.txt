cmake_minimum_required(VERSION 3.5)
project(Z_TPGrupal)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        ../../libs/xml/pugiconfig.hpp
        ../../libs/xml/pugixml.cpp
        ../../libs/xml/pugixml.hpp
        bullet.cpp
        bullet.h
        cell.cpp
        cell.h
        compass.cpp
        compass.h
        controlUnit.cpp
        controlUnit.h
        factory.cpp
        factory.h
        main.cpp
        map.cpp
        map.h
        MapLoader.cpp
        MapLoader.h
        node.cpp
        node.h
        Occupant.cpp
        Occupant.h
        position.cpp
        position.h
        size.cpp
        size.h
        teamable.cpp
        teamable.h
        Terrain.cpp
        Terrain.h
        unit.cpp
        unit.h
        unitMold.cpp
        unitMold.h
        weapon.cpp
        weapon.h
        command.cpp
        command.h
        game.cpp
        game.h
        lobby.cpp
        lobby.h
        menu.cpp
        server.cpp
        server.h
        menu.h
        player.h
        player.cpp territory.cpp territory.h team.cpp team.h playerInfo.cpp playerInfo.h commandMonitor.cpp commandMonitor.h)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_executable(Z_Server ${SOURCE_FILES})

target_link_libraries(Z_Server
        common
        Threads::Threads)
