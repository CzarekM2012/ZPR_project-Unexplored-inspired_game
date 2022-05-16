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

## Building
All following commands should be executed from within the main project directory that contains this README.

### On Linux there are some dependecies that need to be installed first:
- libxrandr-dev
- libudev-dev
- libfreetype-dev
- libopenal-dev
- libvorbis-dev
- libflac-dev
- libxinerama-dev
- libxcursor-dev

### All can be installed at once with apt using a script located in 'scripts' folder:
    scripts/download_dependencies.sh
    
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
.\release\ZPR_Game.exe or ./release/ZPR_Game

### Run unit tests:
.\tests\tests.exe or ./tests/tests


## Controls

### Controller
Left analog stick should control movement, while the right one controls rotation

### Keyboard (debug only)
Use WSAD to move and Q/E to rotate left/right