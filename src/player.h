#pragma once
#include "entity.h"
#include <vector>

class Player 
{
public:
    Player(b2WorldId worldId, int screenWidth, int screenHeight);
    ~Player();

    void Update(float deltaTime);
    void Draw() const;
    b2Vec2 GetPosition() const;

private:
    Entity AttachWeapon(b2WorldId worldId, b2BodyId playerId, Texture2D texture, b2Vec2 extent, b2Polygon polygon);
    void FollowCursor();

    std::vector<Entity> m_playerEntities;
    Entity m_weaponEntity;

    Texture2D m_playerTexture;
    Texture2D m_swordTexture;
};