#pragma once
#include "box2d/box2d.h"
#include "Enemy.h"
#include "Player.h"
#include "environment.h"

class Level
{
public:
    Level(b2WorldId worldId, int screenWidth, int screenHeight);

    void ProcessHits() const;
    void Update(float deltaTime);
    void Draw() const;

private:
    // Order matters: these will be initialized in the order declared here
    b2WorldId worldId;
    Environment m_environment;
    Player m_player;
    std::vector<Enemy> m_enemies;
    Texture2D m_enemySharedTexture;
};