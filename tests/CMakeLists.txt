set(TESTS_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_subdirectory(${EXTERNAL_SOURCE_DIR}/Catch2 ${EXTERNAL_BINARY_DIR}/Catch2)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -pedantic -Werror -faligned-new -O0 -lgcov --coverage)
endif()

add_executable(tests
    main.cpp
    test.cpp
    Angle.cpp
    Entity.cpp
    GameController.cpp
    InputHandler.cpp
    Item.cpp
    ObjectClasses.cpp
    PhysicalObject.cpp
    Player.cpp
    Renderer.cpp
    ${SRCS_DIR}/Angle.cpp
    ${SRCS_DIR}/State.cpp
    ${SRCS_DIR}/GameController.cpp
    ${SRCS_DIR}/InputHandler.cpp
    ${SRCS_DIR}/PhysicalObject.cpp
    ${SRCS_DIR}/ObjectClasses.cpp
    ${SRCS_DIR}/Item.cpp
    ${SRCS_DIR}/Entity.cpp
    ${SRCS_DIR}/Renderer.cpp
    ${SRCS_DIR}/Player.cpp)
    
set_target_properties(tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${TESTS_BINARY_DIR})

target_include_directories(tests PRIVATE
    ${HEADER_DIR})

target_link_libraries(tests PRIVATE
    gcov
    sfml-graphics
    sfml-audio
    sfml-window
    sfml-system
    box2d
    readerwriterqueue
    Catch2::Catch2)

add_dependencies(tests
    sfml-graphics
    sfml-audio
    sfml-window
    sfml-system
    box2d
    readerwriterqueue)

# append directory with Catch module to CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/contrib)
include(CTest)
include(Catch)
catch_discover_tests(tests)

if(WIN32)
    add_custom_command(
        TARGET tests POST_BUILD
        COMMENT "Copying necessary SFML .dll files from ${EXTERNAL_BINARY_DIR}/SFML/lib to ${TESTS_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy ${EXTERNAL_BINARY_DIR}/SFML/lib/sfml-graphics-2.dll
                                         ${EXTERNAL_BINARY_DIR}/SFML/lib/sfml-system-2.dll
                                         ${EXTERNAL_BINARY_DIR}/SFML/lib/sfml-window-2.dll ${TESTS_BINARY_DIR}
        COMMENT "Copying necessary Box2d .dll files from ${EXTERNAL_BINARY_DIR}/Box2d/bin to ${TESTS_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy ${EXTERNAL_BINARY_DIR}/Box2d/bin/libbox2d.dll ${TESTS_BINARY_DIR}
)
endif()
