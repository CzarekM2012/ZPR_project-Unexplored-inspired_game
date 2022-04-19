cmake_minimum_required(VERSION 3.20)

# set the project name
project(
    ZPR_Game
    LANGUAGES CXX)

# set executable and all .dll's to be built directly inside ./release directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/release")

set(EXTERNALS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")
add_subdirectory("${EXTERNALS_DIR}/SFML" EXCLUDE_FROM_ALL)
add_subdirectory("${EXTERNALS_DIR}/Box2d" EXCLUDE_FROM_ALL)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    #add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# add files of main project to the executable
set(SRCS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
add_executable(${PROJECT_NAME}
    "${SRCS_DIR}/test.cpp"
    "${SRCS_DIR}/InputHandler.cpp"
    "${SRCS_DIR}/GameController.cpp"
    "${SRCS_DIR}/Renderer.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-audio sfml-window sfml-system box2d)

add_dependencies(${PROJECT_NAME} sfml-graphics sfml-audio sfml-window sfml-system box2d)
