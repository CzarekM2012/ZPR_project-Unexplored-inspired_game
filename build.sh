cmake -S external -B external/.build -G "MinGW Makefiles"
cmake --build external/.build
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build