# Project: box2d-raylib

A physics engine testbed integrating **Box2D v3** with **raylib** for 2D simulation and rendering.

## Overview

- Single-file C project (`main.c`) demonstrating Box2D physics with raylib rendering
- Uses pixel-space coordinates: 128 pixels per meter (`lengthUnitsPerMeter = 128.0f`)
- 1920×1080 window, 60 FPS target

## Key Concepts

- `Entity` struct pairs a `b2BodyId` with a `b2Vec2 extent` and a `Texture` for rendering
- Box2D bodies use pixel units directly; gravity is scaled accordingly (`9.8 * 128`)
- `DrawEntity` converts Box2D world coordinates to raylib screen coordinates via `b2Body_GetWorldPoint`
- Press `P` to pause/unpause physics simulation

## Build

Uses CMake with `FetchContent` to pull both dependencies:
- Box2D: `https://github.com/erincatto/box2d.git` (main branch)
- raylib: `https://github.com/raysan5/raylib.git` (master branch)

```sh
cmake -B build
cmake --build build
```

Emscripten/WASM build is partially configured (see `CMakeLists.txt`).

## Assets

- `assets/box.png` — texture for dynamic box bodies
- `assets/ground.png` — texture for static ground bodies
- `texture_source.svg` — source SVG for both textures
