### Aleksander Garlikowski, Cezary Moczulski 

## Unexplored-inspired multiplayer PvP game

## Features (WIP):
 - local PvP arena
 - top-down view
 - static camera, limited interface
 - simple, polygon-based character and object models (and matching hitboxes)
 - mainly melee with some ranged (and maybe magical) weapons
 - items provide players with custom actions like lunge or throw
 - requires controllers

## Dependencies
### Windows
All dependencies should be already provided alongside the project, so there should be no need to download anything else.
### Linux
Following software needs to be installed before attempting to build the project:
 - freetype
 - x11
 - xrandr
 - udev
 - opengl
 - flac
 - ogg
 - vorbis
 - vorbisenc
 - vorbisfile
 - openal
 - pthread  

The exact names of the packages may vary from distribution to distribution. Development headers of those packages need to be installed as well.
'scripts/download_dependencies' is a shell script downloading dependencies using apt package manager.

## Building
All following commands should be executed from within the main project directory that contains this README.

### CMake configuration on Linux: 
    cmake -S . -B build -G "Unix Makefiles"

### CMake configuration on Windows: 
(testowane na kompilatorze MinGW W64 4.3.4)

    cmake -S . -B build -G "MinGW Makefiles" 

### Build whole project: 
    cmake --build build 

### Build only the game: 
    cmake --build build --target ZPR_Game 

### Build only tests: 
    cmake --build build --target tests 

### Build documentation: 
    doxygen 

(results can be found in 'docs' folder, eg. .\docs\html\index.html)

## Running and testing

### Run the game:
    .\release\ZPR_Game.exe 
### or:
    ./release/ZPR_Game

### Run unit tests:
    .\tests\bin\tests.exe
### or:
    ./tests/bin/tests


## Controls

### Controller
Left analog stick should control movement, while the right one controls rotation

### Keyboard (debug only)
Use WSAD to move and Q/E to rotate left/right
