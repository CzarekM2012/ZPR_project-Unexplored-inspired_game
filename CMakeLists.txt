cmake_minimum_required(VERSION 3.20)

# set the project name
project(
    ZPR_Game
    LANGUAGES CXX)

set(EXTERNALS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")
set(SRCS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
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

if(NOT EXISTS "${EXTERNALS_DIR}/Box2d/CMakeLists.txt" OR
   NOT EXISTS "${EXTERNALS_DIR}/SFML/CMakeLists.txt" OR
   NOT EXISTS "${EXTERNALS_DIR}/readerwriterqueue/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    execute_process(
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/download_dependencies.sh
    )
endif()

add_subdirectory("${EXTERNALS_DIR}/SFML" EXCLUDE_FROM_ALL)
add_subdirectory("${EXTERNALS_DIR}/Box2d" EXCLUDE_FROM_ALL)
add_subdirectory("${EXTERNALS_DIR}/readerwriterqueue" EXCLUDE_FROM_ALL)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -pedantic -Werror -faligned-new)
endif()

# set executable and all .dll's to be built directly inside ./release directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/release")

# add files of main project to the executable
add_executable(${PROJECT_NAME}
    "${SRCS_DIR}/test.cpp"
    "${SRCS_DIR}/InputHandler.cpp"
    "${SRCS_DIR}/GameController.cpp"
    "${SRCS_DIR}/Renderer.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-audio sfml-window sfml-system box2d)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()

add_dependencies(${PROJECT_NAME} sfml-graphics sfml-audio sfml-window sfml-system box2d readerwriterqueue)
