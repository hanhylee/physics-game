#include "Level.h"
#include "box2d/box2d.h"
#include <vector>

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

    // 1. Load shared visual textures
    m_enemyTexture = LoadTexture("assets/enemy.png");
    m_turretTexture = LoadTexture("assets/turret.png");

    int numTurrets = 10;
    for (int i = 0; i < numTurrets; i++) {
        float randomX = (float)GetRandomValue(-2000, 2000);
        float randomY = (float)GetRandomValue(-2000, 2000);
        b2Vec2 randomPos = { randomX, randomY };

        m_turrets.push_back(std::make_unique<Turret>(m_worldId, randomPos, m_turretTexture));
    }
}

Level::~Level() {
    UnloadTexture(m_enemyTexture);
    UnloadTexture(m_turretTexture);
}

void Level::Update(float deltaTime) {
    m_player.Update(deltaTime);
    ProcessHits();
    ClearDestroyedBodies();
    UpdateEnemies(deltaTime, m_player.GetPosition());
    UpdateCamera(deltaTime, m_player.GetPosition());
    // m_environment.Update(deltaTime); // Add later if boxes need logic (e.g. conveyor belts)
}

void Level::ProcessHits() const {
    b2ContactEvents contactEvents = b2World_GetContactEvents(m_worldId);

    for (int i = 0; i < contactEvents.hitCount; ++i) {
        b2ContactHitEvent *hitEvent = contactEvents.hitEvents + i;

        b2BodyId bodyA = b2Shape_GetBody(hitEvent->shapeIdA);
        b2BodyId bodyB = b2Shape_GetBody(hitEvent->shapeIdB);

        // 1. Process Turret Destruction mechanics
        HandleTurretImpact(hitEvent->shapeIdA, hitEvent->shapeIdB, hitEvent->approachSpeed);

        // 2. Process Kinetic Repulsion / Chain Explosions
        if (hitEvent->approachSpeed > 2.0f) {
            HandleChainExplosion(bodyA, bodyB, hitEvent->approachSpeed, hitEvent->normal);
        }
    }
}

void Level::HandleChainExplosion(b2BodyId bodyA, b2BodyId bodyB, float approachSpeed, b2Vec2 normal) const {
    TraceLog(LOG_INFO, "💥 Kinetic Impact: %0.2f m/s", approachSpeed);

    float massA = b2Body_GetMass(bodyA);
    float massB = b2Body_GetMass(bodyB);

    if (massA + massB <= 0.0f) return;

    // Scale force multiplier exponentially for faster hits
    float forceMultB = 10000.0f * approachSpeed * massB / (massA + massB);

    b2Vec2 impulse = {
        normal.x * forceMultB,
        normal.y * forceMultB
    };

    b2Body_ApplyLinearImpulseToCenter(bodyA, impulse, true);

    b2Vec2 impulseB = {-impulse.x, -impulse.y};
    b2Body_ApplyLinearImpulseToCenter(bodyB, impulseB, true);
}

void Level::HandleTurretImpact(b2ShapeId shapeA, b2ShapeId shapeB, float approachSpeed) const {
    b2Filter filterA = b2Shape_GetFilter(shapeA);
    b2Filter filterB = b2Shape_GetFilter(shapeB);

    // Identify if the clash involves an ENEMY and a TURRET
    bool isEnemyAndTurret = (filterA.categoryBits == CAT_ENEMY && filterB.categoryBits == CAT_TURRET) ||
                            (filterA.categoryBits == CAT_TURRET && filterB.categoryBits == CAT_ENEMY);

    if (isEnemyAndTurret && approachSpeed > 10.0f) {
        TraceLog(LOG_INFO, "🎯 TARGET ACQUIRED: Turret smashed by baseball-enemy at %0.2f m/s!", approachSpeed);

        b2BodyId enemyBody = (filterA.categoryBits == CAT_ENEMY) ? b2Shape_GetBody(shapeA) : b2Shape_GetBody(shapeB);
        b2BodyId turretBody = (filterA.categoryBits == CAT_TURRET) ? b2Shape_GetBody(shapeA) : b2Shape_GetBody(shapeB);

        // Queue them up safely for deletion outside the contact loop
        m_bodiesToDestroy.push_back(enemyBody);
        m_bodiesToDestroy.push_back(turretBody);
    }
}

void Level::UpdateCamera(float deltaTime, b2Vec2 playerPos) {
    float lerpSpeed = 5.0f;

    // Smoothly ease the camera target towards the player's rigid body coordinates
    m_camera.target.x += (playerPos.x - m_camera.target.x) * lerpSpeed * deltaTime;
    m_camera.target.y += (playerPos.y - m_camera.target.y) * lerpSpeed * deltaTime;
}

void Level::ClearDestroyedBodies() {
    for (b2BodyId bodyId : m_bodiesToDestroy) {
        if (b2Body_IsValid(bodyId)) {
            b2DestroyBody(bodyId);

            for (int i = (int)m_turrets.size() - 1; i >= 0; --i) {
                if (B2_ID_EQUALS(m_turrets[i]->GetBodyId(), bodyId)) {
                    m_turrets.erase(m_turrets.begin() + i);
                }
            }

            for (int i = (int)m_enemies.size() - 1; i >= 0; --i) {
                if (B2_ID_EQUALS(m_enemies[i]->GetBodyId(), bodyId)) {
                    m_enemies.erase(m_enemies.begin() + i);
                }
            }
        }
    }
    // Flush the queue so it's clean for the next frame
    m_bodiesToDestroy.clear();
}

void Level::UpdateEnemies(float deltaTime, b2Vec2 playerPos) {
    // 1. Collect spawn requests safely from all turrets
    for (auto& turret : m_turrets) {
        turret->Update(deltaTime, playerPos, m_enemiesToSpawn);
    }

    // 2. Process and empty the spawn queue safely OUTSIDE the turret loop
    for (const auto& request : m_enemiesToSpawn) {
        m_enemies.push_back(std::make_unique<Enemy>(m_worldId, m_enemyTexture, request.position));
    }
    m_enemiesToSpawn.clear(); // Reset for next frame

    // 3. Update all existing enemies
    for (auto& enemy : m_enemies) {
        enemy->Update(deltaTime, playerPos);
    }
}

void Level::Draw() const {
    BeginMode2D(m_camera);
    m_environment.Draw();
    m_player.Draw();
    for (const auto& turret : m_turrets) {
        turret->Draw();
    }
    for (const auto& enemy : m_enemies) {
        enemy->Draw();
    }
    EndMode2D();
}