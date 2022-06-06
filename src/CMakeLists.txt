# add files of main project to the executable
add_executable(${PROJECT_NAME}
    Angle.cpp
    State.cpp
    GameController.cpp
    InputHandler.cpp
    PhysicalObject.cpp
    ObjectClasses.cpp
    Item.cpp
    Entity.cpp
    Player.cpp
    Renderer.cpp
    main.cpp)

#set executable to be built directly inside ../release directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${RELEASE_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${HEADER_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-audio
    sfml-window
    sfml-system
    box2d
    readerwriterqueue)

add_dependencies(${PROJECT_NAME}
    sfml-graphics
    sfml-audio
    sfml-window
    sfml-system
    box2d
    readerwriterqueue)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} PRIVATE
        pthread)
endif()

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
