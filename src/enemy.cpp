#include "Enemy.h"
#include "raymath.h"

Enemy::Enemy(b2WorldId worldId, int screenWidth, int screenHeight)
{
    // 1. Load Enemy Textures
    m_enemyTexture = LoadTexture("assets/enemy.png");
    b2Vec2 enemyExtent = { 0.25f * m_enemyTexture.width, 0.25f * m_enemyTexture.height };
    b2Polygon playerPolygon = b2MakeBox(enemyExtent.x, enemyExtent.y);

    // 2. Create Enemy Entity
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.linearDamping = 3.0f;
    bodyDef.angularDamping = 10.8f;
    bodyDef.position = { 0.5f * screenWidth, screenHeight - 700.0f };
    bodyDef.gravityScale = 0.0f;

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 0.5f;
    shapeDef.material.friction = 0.3f;
    shapeDef.material.restitution = 0.8f;
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

    b2Vec2 direction = b2Sub(playerPosition, myPosition);
    float distance = b2Length(direction);

    if (distance > 5.0f)
    {
        b2Vec2 normalizedDir = b2Normalize(direction);

        // 1. Calculate a force magnitude based on mass so it stays snappy
        float mass = b2Body_GetMass(enemyBodyId);

        // Since m_speed is 100.0f, scale it with mass to push the body
        float forceMagnitude = m_speed * mass * 5.0f;
        b2Vec2 force = b2MulSV(forceMagnitude, normalizedDir);

        // 2. Apply force instead of setting velocity.
        // This lets external impacts (like your sword) override the enemy's path!
        b2Body_ApplyForceToCenter(enemyBodyId, force, true);
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