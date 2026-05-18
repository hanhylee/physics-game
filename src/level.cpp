#include "Level.h"

// Initialize the subsystems in the initializer list
Level::Level(b2WorldId worldId, int screenWidth, int screenHeight)
    : m_environment(worldId, screenWidth, screenHeight),
      m_player(worldId, screenWidth, screenHeight)
{
}

void Level::Update(float deltaTime)
{
    m_player.Update(deltaTime);
    // m_environment.Update(deltaTime); // Add later if boxes need logic (e.g. conveyor belts)
}

void Level::Draw() const
{
    m_environment.Draw();
    m_player.Draw();
}