# Compiler = GCC 7.3.0 mingw 32 (matching SFML version)
cmake_minimum_required(VERSION 3.20)

#set(INCLUDE_DIRECTORY, external/include)

#set(HEADER_FILES ${YOUR_DIRECTORY}/*)

# set the project name
project(ZPR_Game)

# find SFML libraries
#set(CMAKE_MODULE_PATH, "./external/lib/cmake/SFML")
#set(SFML_DIR, "./external/lib/cmake/SFML")
set(sfml_root "${CMAKE_CURRENT_SOURCE_DIR}/resources/SFML-2.5.1")
set(SFML_DIR "${sfml_root}/lib/cmake/SFML")
find_package(SFML "2.5" COMPONENTS audio graphics window system REQUIRED)

# add the executable
set(sources_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")
add_executable(ZPR_Game "${sources_dir}/test.cpp"
                        "${sources_dir}/InputHandler.cpp"
                        "${sources_dir}/GameController.cpp"
                        "${sources_dir}/Renderer.cpp")

# add SFML files to include
target_include_directories(ZPR_Game PRIVATE ${INCLUDE_DIRECTORIES} "${sfml_root}/include")

set(sfml_dlls_dir "${sfml_root}/bin")
target_link_directories(ZPR_Game PRIVATE ${LINK_DIRECTORIES} ${sfml_dlls_dir})
#set(LD_LIBRARY_PATH ${SFML_DIR}/lib)
target_link_libraries(ZPR_Game PRIVATE sfml-graphics-2 sfml-audio-2 sfml-window-2 sfml-system-2)

#add_definitions(-include ${CMAKE_CURRENT_SOURCE_DIR}/external/include/SFML/Window.hpp)

# Copy the executable to /release after finishing the build
set(exec_source "${CMAKE_CACHEFILE_DIR}/ZPR_Game.exe")
set(exec_destination "${CMAKE_SOURCE_DIR}/release/ZPR_Game.exe")

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy "${exec_source}" "${exec_destination}"
    COMMAND ${CMAKE_COMMAND} -E rm "${exec_source}"
    COMMENT "Moving executable from ${exec_source} to ${exec_destination}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${sfml_dlls_dir}" "${CMAKE_SOURCE_DIR}/release"
    COMMENT "Copying .dll's from ${sfml_root}/bin to ${CMAKE_SOURCE_DIR}/release"
)