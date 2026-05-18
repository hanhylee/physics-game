#include "Level.h"

// Initialize the subsystems in the initializer list
Level::Level(b2WorldId worldId, int screenWidth, int screenHeight)
    : m_environment(worldId, screenWidth, screenHeight),
      m_player(worldId, screenWidth, screenHeight)
{
    m_enemySharedTexture = LoadTexture("assets/enemy.png");
    int numEnemies = 3;
    for (int i = 0; i < numEnemies; i++) {
        m_enemies.emplace_back(worldId, m_enemySharedTexture, screenWidth, screenHeight);
    }
}

void Level::Update(float deltaTime)
{
    m_player.Update(deltaTime);
    b2Vec2 playerPos = m_player.GetPosition();
    for (auto& enemy : m_enemies)
    {
        enemy.Update(deltaTime, playerPos);
    }
    // m_environment.Update(deltaTime); // Add later if boxes need logic (e.g. conveyor belts)
}

void Level::Draw() const
{
    m_environment.Draw();
    m_player.Draw();
    for (const auto& enemy : m_enemies)
    {
        enemy.Draw();
    }
}