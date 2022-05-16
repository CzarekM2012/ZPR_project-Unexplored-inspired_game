cmake_minimum_required(VERSION 3.20)

# set the project name
project(
    ZPR_Game
    LANGUAGES CXX)

set(EXTERNAL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)
set(EXTERNAL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/external)
set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/header)
set(SRCS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(RELEASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/release)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS ${PROJECT_SOURCE_DIR}/.git)
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS ${EXTERNAL_SOURCE_DIR}/Box2d/CMakeLists.txt OR
   NOT EXISTS ${EXTERNAL_SOURCE_DIR}/SFML/CMakeLists.txt OR
   NOT EXISTS ${EXTERNAL_SOURCE_DIR}/readerwriterqueue/CMakeLists.txt OR
   NOT EXISTS ${EXTERNAL_SOURCE_DIR}/Catch2/CMakeLists.txt)
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    execute_process(
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/download_dependencies.sh
    )
endif()

# specify configuration which will be used for generating sfml .dll's (affects their names)
set(CMAKE_BUILD_TYPE "Release")
add_subdirectory(${EXTERNAL_SOURCE_DIR}/SFML EXCLUDE_FROM_ALL)
# defining this name controls whether or not Box2d functions will be exported to .dll's properly
add_compile_definitions(box2d_EXPORTS)
add_subdirectory(${EXTERNAL_SOURCE_DIR}/Box2d EXCLUDE_FROM_ALL)
add_subdirectory(${EXTERNAL_SOURCE_DIR}/readerwriterqueue EXCLUDE_FROM_ALL)
add_subdirectory(${EXTERNAL_SOURCE_DIR}/Catch2)

include_directories(${HEADER_DIR})

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -pedantic -Werror -faligned-new)
endif()

add_executable(tests
    ${TESTS_DIR}/main.cpp
    ${TESTS_DIR}/test.cpp)
target_link_libraries(tests PRIVATE
    Catch2::Catch2)
set_target_properties(tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${TESTS_DIR})

# append directory with Catch module to CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/contrib)
include(CTest)
include(Catch)
catch_discover_tests(tests)

# add files of main project to the executable
add_executable(${PROJECT_NAME}
    ${SRCS_DIR}/GameController.cpp
    ${SRCS_DIR}/InputHandler.cpp
    ${SRCS_DIR}/PhysicalObject.cpp
    ${SRCS_DIR}/Renderer.cpp
    ${SRCS_DIR}/main.cpp)

#set executable to be built directly inside ./release directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${RELEASE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-audio
    sfml-window
    sfml-system
    box2d
    readerwriterqueue)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()

add_dependencies(${PROJECT_NAME} sfml-graphics sfml-audio sfml-window sfml-system box2d readerwriterqueue)

if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMENT "Copying necessary SFML .dll files from ${EXTERNAL_BINARY_DIR}/SFML/lib to ${RELEASE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy ${EXTERNAL_BINARY_DIR}/SFML/lib/sfml-graphics-2.dll
                                         ${EXTERNAL_BINARY_DIR}/SFML/lib/sfml-system-2.dll
                                         ${EXTERNAL_BINARY_DIR}/SFML/lib/sfml-window-2.dll ${RELEASE_DIR}
        COMMENT "Copying necessary Box2d .dll files from ${EXTERNAL_BINARY_DIR}/Box2d/bin to ${RELEASE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy ${EXTERNAL_BINARY_DIR}/Box2d/bin/libbox2d.dll ${RELEASE_DIR}
)
endif()
