#include "Level.h"
#include "box2d/box2d.h"
#include <iostream>

// Initialize the subsystems in the initializer list
Level::Level(b2WorldId _worldId, int screenWidth, int screenHeight)
    : m_environment(_worldId, screenWidth, screenHeight),
      m_player(_worldId, screenWidth, screenHeight),
      m_enemy1(_worldId, screenWidth, screenHeight)
{
    worldId = _worldId;
}

void Level::ProcessHits() const {
    b2ContactEvents contactEvents = b2World_GetContactEvents(worldId);
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
    m_player.Update(deltaTime);
    ProcessHits();
    b2Vec2 playerPos = m_player.GetPosition();
    m_enemy1.Update(deltaTime, playerPos);
    // m_environment.Update(deltaTime); // Add later if boxes need logic (e.g. conveyor belts)

}

void Level::Draw() const {
    m_environment.Draw();
    m_player.Draw();
    m_enemy1.Draw();
}
