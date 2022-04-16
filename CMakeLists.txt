cmake_minimum_required(VERSION 3.20)

# set the project name
project(ZPR_Game)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

set(EXTERNALS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

# find packages used in main project
set(SFML_DIR "${EXTERNALS_DIR}/SFML/install/lib/cmake/SFML")
find_package(SFML "2.5" COMPONENTS audio graphics window system REQUIRED)

set(box2d_DIR "${EXTERNALS_DIR}/Box2D/install/lib/cmake/box2d")
find_package(box2d "2.4" REQUIRED)

# add files of main project to the executable
set(SRCS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
add_executable(${PROJECT_NAME}
    "${SRCS_DIR}/test.cpp"
    "${SRCS_DIR}/InputHandler.cpp"
    "${SRCS_DIR}/GameController.cpp"
    "${SRCS_DIR}/Renderer.cpp")

# set target to be built directly in ./release directory
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/release")

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-audio sfml-window sfml-system box2d::box2d)

# Copy necessary .dll's to ./release after finishing the build
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${EXTERNALS_DIR}/SFML/install/bin" "${CMAKE_SOURCE_DIR}/release"
    COMMENT "Copying .dll's from ${SFML_DLLS_DIR}/SFML/install/bin to ${CMAKE_SOURCE_DIR}/release")
