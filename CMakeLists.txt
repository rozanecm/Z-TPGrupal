cmake_minimum_required(VERSION 3.5)
project(Z_TPGrupal)

set(CMAKE_CXX_STANDARD 11)
find_package(PkgConfig)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(SOURCE_FILES
        src/main.cpp src/cell.cpp
        src/cell.h src/Terrain.cpp src/Terrain.h src/Occupant.cpp src/Occupant.h
        src/xml/pugixml.cpp src/xml/pugiconfig.hpp src/xml/pugixml.hpp
        src/MapLoader.cpp src/MapLoader.h src/compass.cpp src/compass.h
        src/lock.cpp src/lock.h src/lock.cpp src/lock.h src/node.cpp src/node.h
        src/size.cpp src/size.h src/map.cpp src/map.h src/unit.cpp src/unit.h
        src/position.cpp src/position.h src/weapon.cpp src/weapon.h
        src/unitMold.cpp src/unitMold.h src/teamable.cpp src/teamable.h
        src/factory.cpp src/factory.h src/bullet.cpp src/bullet.h
        src/thread.cpp src/thread.h src/socket.cpp src/socket.h src/socketError.cpp src/socketError.h src/messenger.cpp src/messenger.h src/controUnit.cpp src/controUnit.h)

add_executable(Z_TPGrupal ${SOURCE_FILES})



target_link_libraries(Z_TPGrupal
        Threads::Threads)