cmake_minimum_required(VERSION 3.20)
include(ExternalProject)

# set the project name
project(ZPR_Game)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

set(EXTERNALS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

# add SFML as external project
set(SFML_DIR "${EXTERNALS_DIR}/SFML")
set(SFML_INSTALL "${SFML_DIR}/install")
ExternalProject_Add(SFML
    PREFIX ${SFML_DIR}
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2f11710abc5aa478503a7ff3f9e654bd2078ebab
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${SFML_INSTALL}
)

set(BOX2D_DIR "${EXTERNALS_DIR}/Box2D")
set(BOX2D_INSTALL "${BOX2D_DIR}/install")
ExternalProject_Add(BOX2D
    PREFIX ${BOX2D_DIR}
    GIT_REPOSITORY https://github.com/erincatto/box2d.git
    GIT_TAG 9ebbbcd960ad424e03e5de6e66a40764c16f51bc
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${BOX2D_INSTALL}
)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -pedantic -Werror -faligned-new)
endif()

# add files of main project to the executable
set(SRCS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
add_executable(${PROJECT_NAME} "${SRCS_DIR}/test.cpp"
                        "${SRCS_DIR}/InputHandler.cpp"
                        "${SRCS_DIR}/GameController.cpp"
                        "${SRCS_DIR}/Renderer.cpp"
                        "${SRCS_DIR}/PhysicalObject.cpp")

add_dependencies(${PROJECT_NAME} SFML BOX2D)

set(SFML_DIR "${SFML_INSTALL}/lib/cmake/SFML")
find_package(SFML "2.5" COMPONENTS audio graphics window system REQUIRED)

set(box2d_DIR "${BOX2D_INSTALL}/lib/cmake/box2d")
find_package(box2d "2.4" REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-audio sfml-window sfml-system box2d::box2d)

# add SFML files to include
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRECTORIES} "${SFML_INSTALL}/include" "${BOX2D_INSTALL}/include")

set(SFML_DLLS_DIR "${SFML_INSTALL}/bin")
#target_link_directories(ZPR_Game PRIVATE ${LINK_DIRECTORIES} ${SFML_DLLS_DIR})

# Copy the executable and .dll's to /release after finishing the build
set(EXE_SRC "${CMAKE_CACHEFILE_DIR}/ZPR_Game.exe")
set(EXE_DST "${CMAKE_SOURCE_DIR}/release/ZPR_Game.exe")

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${EXE_SRC}" "${EXE_DST}"
    COMMAND ${CMAKE_COMMAND} -E rm "${EXE_SRC}"
    COMMENT "Moving executable from ${EXE_SRC} to ${EXE_DST}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SFML_DLLS_DIR}" "${CMAKE_SOURCE_DIR}/release"
    COMMENT "Copying .dll's from ${SFML_DLLS_DIR} to ${CMAKE_SOURCE_DIR}/release"
)
