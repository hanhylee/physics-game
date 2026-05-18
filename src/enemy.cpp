#include "Enemy.h"
#include "raymath.h"

Enemy::Enemy(b2WorldId worldId, int screenWidth, int screenHeight)
{
    // 1. Load Enemy Textures
    m_enemyTexture = LoadTexture("assets/enemy.png");

    b2Vec2 enemyExtent = { 0.25f * m_enemyTexture.width, 0.25f * m_enemyTexture.height };

    b2Polygon playerPolygon = b2MakeBox(enemyExtent.x, enemyExtent.y);

    // 2. Create Enemy Entity
    // Using a static drop-in height since the enemy doesn't know how tall the ground is anymore
    float spawnX = 0.5f * screenWidth - 3.0f * enemyExtent.x;
    float spawnY = screenHeight - 700.0f;

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.angularDamping = 10.8f;
    bodyDef.position = { spawnX, spawnY };
    bodyDef.gravityScale = 0.0f;

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(bodyId, &shapeDef, &playerPolygon);
    m_speed = 100.0f;

    m_enemyEntities.push_back({ bodyId, enemyExtent, m_enemyTexture, 0.5f });
}

Enemy::~Enemy()
{
    // Clean up enemy assets
    UnloadTexture(m_enemyTexture);
}

void Enemy::FollowPlayer(float deltaTime, b2Vec2 playerPosition)
{
    if (m_enemyEntities.empty()) return;

    b2BodyId enemyBodyId = m_enemyEntities[0].bodyId;
    b2Vec2 myPosition = b2Body_GetPosition(enemyBodyId);

    // 1. Calculate vector pointing from enemy to player
    b2Vec2 direction = b2Sub(playerPosition, myPosition);
    float distance = b2Length(direction);

    // 2. Set a comfortable detection range (e.g., don't move if closer than 5 pixels)
    if (distance > 5.0f)
    {
        b2Vec2 normalizedDir = b2Normalize(direction);

        b2Vec2 targetVelocity = b2MulSV(m_speed, normalizedDir);

        // Direct velocity assignment overrides floating drift!
        b2Body_SetLinearVelocity(enemyBodyId, targetVelocity);
    }
    else
    {
        // 3. Force the enemy to stop dead in its tracks when it reaches you
        b2Body_SetLinearVelocity(enemyBodyId, {0.0f, 0.0f});
    }
}

void Enemy::Update(float deltaTime, b2Vec2 playerPosition)
{
    FollowPlayer(deltaTime, playerPosition);
    // Add jumping, dashing, or health checks here later
}

void Enemy::Draw() const
{
    for (const auto& enemy : m_enemyEntities) enemy.Draw();
}