cmake_minimum_required(VERSION 3.5)
project(Z_TPGrupal)

set(CMAKE_CXX_STANDARD 11)
find_package(PkgConfig)

set(LIBS_DIR libs)
set(PUGI ${LIBS_DIR}/xml)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(SOURCE_FILES
        libs/xml/pugixml.hpp libs/xml/pugixml.cpp
        src/server/main.cpp
        src/server/cell.cpp src/server/cell.h
        src/server/Terrain.cpp src/server/Terrain.h
        src/server/Occupant.cpp src/server/Occupant.h
        src/server/MapLoader.cpp src/server/MapLoader.h
        src/server/compass.cpp src/server/compass.h
        src/Lock.cpp src/Lock.h
        src/server/node.cpp src/server/node.h
        src/server/size.cpp src/server/size.h
        src/server/map.cpp src/server/map.h
        src/server/unit.cpp src/server/unit.h
        src/server/position.cpp src/server/position.h
        src/server/weapon.cpp src/server/weapon.h
        src/server/unitMold.cpp src/server/unitMold.h
        src/server/teamable.cpp src/server/teamable.h
        src/server/factory.cpp src/server/factory.h
        src/server/bullet.cpp src/server/bullet.h
        src/Thread.cpp src/Thread.h
        src/socket.cpp src/socket.h
        src/socketError.cpp src/socketError.h
        src/messenger.cpp src/messenger.h
        src/server/controlUnit.cpp src/server/controlUnit.h
        src/server/command.cpp src/server/command.h
        src/server/server.cpp src/server/server.h
        src/server/player.cpp src/server/player.h
        src/server/menu.cpp src/server/menu.h
        src/server/lobby.cpp src/server/lobby.h
        src/server/game.cpp src/server/game.h)

add_executable(Z_TPGrupal ${SOURCE_FILES})



target_link_libraries(Z_TPGrupal
        Threads::Threads)