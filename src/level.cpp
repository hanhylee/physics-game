#include "Level.h"
#include "box2d/box2d.h"
#include <iostream>

// Initialize the subsystems in the initializer list
Level::Level(b2WorldId _worldId, int screenWidth, int screenHeight)
    : m_environment(_worldId, screenWidth, screenHeight),
      m_player(_worldId, screenWidth, screenHeight),
      m_worldId(_worldId)
{
    m_camera = { 0 };
    m_camera.target = (Vector2){ 0.0f, 0.0f };
    m_camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    m_camera.rotation = 0.0f;
    m_camera.zoom = 0.25f;

    m_enemySharedTexture = LoadTexture("assets/enemy.png");
    int numEnemies = 3;
    for (int i = 0; i < numEnemies; i++) {
        m_enemies.emplace_back(m_worldId, m_enemySharedTexture, screenWidth, screenHeight);
    }
}

void Level::ProcessHits() const {
    b2ContactEvents contactEvents = b2World_GetContactEvents(m_worldId);
    for (int i = 0; i < contactEvents.hitCount; ++i) {
        b2ContactHitEvent *hitEvent = contactEvents.hitEvents + i;
        if (hitEvent->approachSpeed > 5.0f) {
            TraceLog(LOG_INFO, "Hit: %d m/s", hitEvent->approachSpeed);
            b2BodyId bodyA = b2Shape_GetBody(hitEvent->shapeIdA);
            b2BodyId bodyB = b2Shape_GetBody(hitEvent->shapeIdB);
            float massA = b2Body_GetMass(bodyA);
            float massB = b2Body_GetMass(bodyB);
            float forceMultB = 2000.0f * hitEvent->approachSpeed * massB / (massA + massB);
            b2Vec2 impulse = {
                hitEvent->normal.x * forceMultB,
                hitEvent->normal.y * forceMultB
            };
            b2Body_ApplyLinearImpulseToCenter(bodyA, impulse, true);

            b2Vec2 impulseB = {-impulse.x, -impulse.y};
            b2Body_ApplyLinearImpulseToCenter(bodyB, impulseB, true);
        }
    }
}

void Level::Update(float deltaTime) {

    b2Vec2 playerPos = m_player.GetPosition();
    float lerpSpeed = 5.0f;
    m_camera.target.x += (playerPos.x - m_camera.target.x) * lerpSpeed * deltaTime;
    m_camera.target.y += (playerPos.y - m_camera.target.y) * lerpSpeed * deltaTime;
    m_player.Update(deltaTime);
    for (auto& enemy : m_enemies)
    {
        enemy.Update(deltaTime, playerPos);
    }
    ProcessHits();
    // m_environment.Update(deltaTime); // Add later if boxes need logic (e.g. conveyor belts)

}

void Level::Draw() const {
    BeginMode2D(m_camera);
    m_environment.Draw();
    m_player.Draw();
    for (const auto& enemy : m_enemies)
    {
        enemy.Draw();
    }
    EndMode2D();
}