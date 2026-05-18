#pragma once
#include "box2d/box2d.h"
#include "Player.h"
#include "environment.h"

class Level
{
public:
    Level(b2WorldId worldId, int screenWidth, int screenHeight);

    void Update(float deltaTime);
    void Draw() const;

private:
    // Order matters: these will be initialized in the order declared here
    Environment m_environment;
    Player m_player;
    b2WorldId worldId;
};