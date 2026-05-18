#pragma once
#include "entity.h"
#include <vector>

class Enemy
{
public:
    Enemy(b2WorldId worldId, Texture2D texture, int screenWidth, int screenHeight);
    ~Enemy();

    void Update(float deltaTime, b2Vec2 playerPosition);
    void Draw() const;

private:
    void FollowPlayer(float deltaTime, b2Vec2 playerPosition);

    std::vector<Entity> m_enemyEntities;
    int m_speed;

    Texture2D m_enemyTexture;
};