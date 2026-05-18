#pragma once
#include "box2d/box2d.h"
#include "raylib.h"
#include <vector>

struct EnemySpawnRequest {
    b2Vec2 position;
};

class Turret {
public:
    Turret(b2WorldId worldId, b2Vec2 position, Texture2D turretTex);
    
    void Update(float deltaTime,
                    b2Vec2 playerPos,
                    std::vector<EnemySpawnRequest>& spawnQueue);
    void Draw() const;
    [[nodiscard]] b2BodyId GetBodyId() const;

private:
    b2BodyId m_bodyId;
    b2Vec2 m_position;
    Texture2D m_texture;
    b2WorldId m_worldId;
    
    float m_fireRate = 2.0f; // Seconds between shots
    float m_fireTimer = 0.0f;
};