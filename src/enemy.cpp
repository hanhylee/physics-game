#include "Enemy.h"
#include "raymath.h"

Enemy::Enemy(b2WorldId worldId, Texture2D texture, int screenWidth, int screenHeight)
{
    // 1. Load Enemy Textures
    m_enemyTexture = texture;
    b2Vec2 enemyExtent = { 0.25f * m_enemyTexture.width, 0.25f * m_enemyTexture.height };
    b2Polygon playerPolygon = b2MakeBox(enemyExtent.x, enemyExtent.y);

    // 2. Create Enemy Entity
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.linearDamping = 1.5f;
    bodyDef.angularDamping = 10.8f;
    bodyDef.gravityScale = 0.0f;
    float padding = enemyExtent.x * 2.0f;
    float randomX = (float)GetRandomValue((int)padding, (int)(screenWidth - padding));
    float spawnY = 50.0f;
    bodyDef.position = { randomX, spawnY };

    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 0.5f;
    shapeDef.material.friction = 0.1f;
    shapeDef.material.restitution = 0.9f;
    shapeDef.filter.categoryBits = CAT_ENEMY;
    shapeDef.filter.maskBits = CAT_PLAYER | CAT_WEAPON | CAT_ENEMY | CAT_TURRET | CAT_WALL;
    shapeDef.enableContactEvents = true;
    b2CreatePolygonShape(bodyId, &shapeDef, &playerPolygon);
    m_speed = 500.0f;

    m_enemyEntities.push_back({ bodyId, enemyExtent, m_enemyTexture, 0.5f });
}

Enemy::Enemy(b2WorldId worldId, Texture2D texture, b2Vec2 spawnPosition)
{
    // 1. Load Enemy Textures
    m_enemyTexture = texture;
    b2Vec2 enemyExtent = { 0.25f * m_enemyTexture.width, 0.25f * m_enemyTexture.height };
    b2Polygon playerPolygon = b2MakeBox(enemyExtent.x, enemyExtent.y);

    // 2. Create Enemy Entity
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.linearDamping = 1.5f;
    bodyDef.angularDamping = 10.8f;
    bodyDef.gravityScale = 0.0f;
    m_speed = 500.0f;

    // 3. Spawn Body at Turret Position
    bodyDef.position = spawnPosition;
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    // 4. Shape Setup
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 0.5f;
    shapeDef.material.friction = 0.1f;
    shapeDef.material.restitution = 0.9f;
    shapeDef.filter.categoryBits = CAT_ENEMY;
    shapeDef.filter.maskBits = CAT_PLAYER | CAT_WEAPON | CAT_ENEMY | CAT_TURRET | CAT_WALL;
    shapeDef.enableContactEvents = true;
    b2CreatePolygonShape(bodyId, &shapeDef, &playerPolygon);

    m_enemyEntities.push_back({ bodyId, enemyExtent, m_enemyTexture, 0.5f });
}

Enemy::~Enemy()
{
    // Clean up enemy assets
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
        // scale m_speed with mass to push the body
        float mass = b2Body_GetMass(enemyBodyId);
        float forceMagnitude = m_speed * mass;
        b2Vec2 force = b2MulSV(forceMagnitude, normalizedDir);

        // 2. Apply force instead of setting velocity.
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

b2BodyId Enemy::GetBodyId() const {
    if (m_enemyEntities.empty()) return b2_nullBodyId;
    return m_enemyEntities[0].bodyId;
}