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

# specify configuration which will be used for generating sfml .dll's (affects their names)
set(CMAKE_BUILD_TYPE "Release")
add_subdirectory(${EXTERNAL_SOURCE_DIR}/SFML EXCLUDE_FROM_ALL)
# defining this name controls whether or not Box2d functions will be exported to .dll's properly
add_compile_definitions(box2d_EXPORTS)
add_subdirectory(${EXTERNAL_SOURCE_DIR}/Box2d EXCLUDE_FROM_ALL)
add_subdirectory(${EXTERNAL_SOURCE_DIR}/readerwriterqueue EXCLUDE_FROM_ALL)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -pedantic -Werror -faligned-new)
endif()

add_subdirectory(${SRCS_DIR})
add_subdirectory(${TESTS_DIR})
