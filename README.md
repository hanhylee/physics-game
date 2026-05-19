# physics sim

## credits and resources

this project was originally forked from erin catto's box2d-raylib github repo
- https://github.com/erincatto/box2d-raylib

Box2D + Raylib are used to simulate 2d physics. Raylib provides the windowing and drawing while Box2D provides rigid body physics.
- https://www.raylib.com/
- https://box2d.org/

CMake and FetchContent are used to bring together box2d and raylib with no submodules, packages, or file copying.
- https://cmake.org/
- https://cmake.org/cmake/help/latest/guide/user-interaction/index.html
- https://code.visualstudio.com/docs/cpp/cmake-linux

Emscripten is used to compile the project to WebAssembly
- https://emscripten.org/

## windows setup

1. get the emsdk repo
```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
```

2. download and install latest SDK tools
```bash
./emsdk install latest
```

3. activate path and other environment variables
```bash
emdsk_env.ps1
```

4. go to project directory and run build commands
```bash
emcmake cmake . -B cmake-build-emscripten -G Ninja
cmake --build cmake-build-emscripten
```

4. run game
```bash
emrun cmake-build-emscripten/game.html
```