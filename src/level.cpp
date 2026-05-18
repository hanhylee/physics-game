#include "Level.h"

// Initialize the subsystems in the initializer list
Level::Level(b2WorldId worldId, int screenWidth, int screenHeight)
    : m_environment(worldId, screenWidth, screenHeight),
      m_player(worldId, screenWidth, screenHeight),
      m_enemy1(worldId, screenWidth, screenHeight)
{
}

void Level::Update(float deltaTime)
{
    m_player.Update(deltaTime);
    b2Vec2 playerPos = m_player.GetPosition();
    m_enemy1.Update(deltaTime, playerPos);
    // m_environment.Update(deltaTime); // Add later if boxes need logic (e.g. conveyor belts)
}

void Level::Draw() const
{
    m_environment.Draw();
    m_player.Draw();
    m_enemy1.Draw();
}