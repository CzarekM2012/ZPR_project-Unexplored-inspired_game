# Compiler = GCC 7.3.0 mingw 32 (matching SFML version)
cmake_minimum_required(VERSION 3.20)

#set(INCLUDE_DIRECTORY, external/include)

#set(HEADER_FILES ${YOUR_DIRECTORY}/*)

# set the project name
project(ZPR_Game)

# find SFML libraries
#set(CMAKE_MODULE_PATH, "./external/lib/cmake/SFML")
#set(SFML_DIR, "./external/lib/cmake/SFML")
set(SFML_DIR, "C:/Program Files (x86)/SFML/SFML-2.5.1")
find_package(SFML 2.5 COMPONENTS audio graphics window system REQUIRED)

# add the executable
add_executable(ZPR_Game src/test.cpp src/InputHandler.cpp src/GameController.cpp src/Renderer.cpp)

# add SFML files to include
target_include_directories(ZPR_Game PRIVATE ${INCLUDE_DIRECTORIES} "C:/Program Files (x86)/SFML/SFML-2.5.1/include")

target_link_directories(ZPR_Game PRIVATE ${LINK_DIRECTORIES} "C:/Program Files (x86)/SFML/SFML-2.5.1/bin")
#set(LD_LIBRARY_PATH ${SFML_DIR}/lib)
target_link_libraries(ZPR_Game PRIVATE sfml-graphics-2 sfml-audio-2 sfml-window-2 sfml-system-2)

#add_definitions(-include ${CMAKE_CURRENT_SOURCE_DIR}/external/include/SFML/Window.hpp)

# Copy the executable to /release after finishing the build
set(copy_source_dir "${CMAKE_SOURCE_DIR}/build/ZPR_Game.exe")
set(copy_dest_dir "${CMAKE_SOURCE_DIR}/release/ZPR_Game.exe")
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy "${copy_source_dir}" "${copy_dest_dir}"
    COMMENT "Copying files from ${copy_source_dir} to ${copy_dest_dir}"
)