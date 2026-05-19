#pragma once
#include "box2d/box2d.h"
#include "Enemy.h"
#include "Player.h"
#include "Environment.h"
#include "Turret.h"
#include <iostream>
#include <memory>

class Level
{
public:
    Level(b2WorldId worldId, int screenWidth, int screenHeight);
    ~Level();

    void ProcessHits() const;
    void Update(float deltaTime);
    void Draw() const;
    int GetEnemyCount() const { return (int)m_enemies.size(); }

private:
    // Order matters: these will be initialized in the order declared here
    Environment m_environment;
    Player m_player;
    b2WorldId m_worldId;
    Camera2D m_camera;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Turret>> m_turrets;

    void HandleChainExplosion(b2BodyId bodyA, b2BodyId bodyB, float approachSpeed, b2Vec2 normal) const;
    void HandleTurretImpact(b2ShapeId shapeA, b2ShapeId shapeB, float approachSpeed) const;

    void ClearDestroyedBodies();
    void UpdateCamera(float deltaTime, b2Vec2 playerPos);
    void UpdateEnemies(float deltaTime, b2Vec2 playerPos);

    std::vector<EnemySpawnRequest> m_enemiesToSpawn;
    mutable std::vector<b2BodyId> m_bodiesToDestroy;

    Texture2D m_enemyTexture;
    Texture2D m_turretTexture;
};
