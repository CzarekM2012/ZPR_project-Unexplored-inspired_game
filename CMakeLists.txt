cmake_minimum_required(VERSION 3.20)
include(ExternalProject)

# set the project name
project(ZPR_Game)

# add SFML as external project
set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/SFML")
set(SFML_INSTALL "${SFML_DIR}/install")
ExternalProject_Add(SFML
    PREFIX ${SFML_DIR}
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2f11710abc5aa478503a7ff3f9e654bd2078ebab
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${SFML_INSTALL}
)

# add files of main project to the executable
set(SRCS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
add_executable(${PROJECT_NAME} "${SRCS_DIR}/test.cpp"
                        "${SRCS_DIR}/InputHandler.cpp"
                        "${SRCS_DIR}/GameController.cpp"
                        "${SRCS_DIR}/Renderer.cpp")

add_dependencies(${PROJECT_NAME} SFML)

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics-2 sfml-audio-2 sfml-window-2 sfml-system-2)

# add SFML files to include
target_include_directories(ZPR_Game PRIVATE ${INCLUDE_DIRECTORIES} "${SFML_INSTALL}/include")

set(SFML_DLLS_DIR "${SFML_INSTALL}/bin")
target_link_directories(ZPR_Game PRIVATE ${LINK_DIRECTORIES} ${SFML_DLLS_DIR})

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
