include(ExternalProject)

set(SFML_SRC "${CMAKE_CURRENT_SOURCE_DIR}/SFML")
set(SFML_INSTALL "${SFML_SRC}/install")
set(SFML_DIR "${SFML_INSTALL}/lib/cmake/SFML")
find_package(SFML "2.5" COMPONENTS audio graphics window system)
# add SFML as external project
if(NOT SFML_FOUND)
    ExternalProject_Add(SFML
        PREFIX ${SFML_SRC}
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2f11710abc5aa478503a7ff3f9e654bd2078ebab
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${SFML_INSTALL}
    )
endif()

set(box2d_SRC "${CMAKE_CURRENT_SOURCE_DIR}/Box2D")
set(Box2D_INSTALL "${box2d_SRC}/install")
set(box2d_DIR "${Box2D_INSTALL}/lib/cmake/box2d")
find_package(box2d "2.4")
if(NOT box2d_FOUND)
    ExternalProject_Add(BOX2D
        PREFIX ${box2d_SRC}
        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        GIT_TAG 9ebbbcd960ad424e03e5de6e66a40764c16f51bc
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${Box2D_INSTALL}
    )
endif()