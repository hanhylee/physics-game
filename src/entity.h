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

enum CollisionCategory {
    CAT_PLAYER    = 0x0001,
    CAT_ENEMY     = 0x0002,
    CAT_WEAPON    = 0x0004,
    CAT_PROJECTILE = 0x0008,
    CAT_TURRET    = 0x0010,
    CAT_WALL      = 0x0020
};