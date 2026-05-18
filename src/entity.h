#pragma once
#include "raylib.h"
#include "box2d/box2d.h"

constexpr float LENGTH_UNITS_PER_METER = 128.0f;

struct Entity 
{
    b2BodyId bodyId;
    b2Vec2 extent;
    Texture2D texture;
    float scale;

    void Draw() const;
};