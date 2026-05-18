#include "Turret.h"
#include "Entity.h"

Turret::Turret(b2WorldId worldId, b2Vec2 position, Texture2D turretTex)
    : m_position(position), m_texture(turretTex)
{
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_staticBody; // Static means unmovable fortress
    bodyDef.position = position;
    m_bodyId = b2CreateBody(worldId, &bodyDef);
    m_worldId = worldId;

    b2Vec2 extent = { 0.25f * m_texture.width, 0.25f * m_texture.height };
    b2Polygon box = b2MakeBox(extent.x, extent.y);

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.filter.categoryBits = CAT_TURRET;
    shapeDef.filter.maskBits = CAT_ENEMY; // Only enemies should trigger physics resolution
    shapeDef.enableContactEvents = true;
    
    b2CreatePolygonShape(m_bodyId, &shapeDef, &box);
}

void Turret::Update(float deltaTime,
                    b2Vec2 playerPos,
                    std::vector<EnemySpawnRequest>& spawnQueue) {
    m_fireTimer += deltaTime;

    if (m_fireTimer >= m_fireRate) {
        m_fireTimer = 0.0f;

        b2Vec2 livePosition = b2Body_GetPosition(m_bodyId);
        b2Vec2 dir = b2Normalize(b2Sub(playerPos, livePosition));

        b2Vec2 spawnPosition = b2Add(livePosition, b2MulSV(2.0f, dir));

        spawnQueue.push_back({ spawnPosition });
    }
}

void Turret::Draw() const
{
    // 1. Safety check to make sure the physics body actually exists
    if (!b2Body_IsValid(m_bodyId)) return;

    // 2. Fetch the current position from the Box2D world space
    b2Vec2 position = b2Body_GetPosition(m_bodyId);

    // 3. Define the center pivot origin for rotating/scaling the texture
    Vector2 origin = {
        0.25f * m_texture.width,
        0.25f * m_texture.height
    };

    // 4. Set up the destination rectangle where it will draw on screen
    Rectangle dest = {
        position.x,
        position.y,
        0.5f * m_texture.width,  // Scale width down by 50% to match physical size
        0.5f * m_texture.height  // Scale height down by 50%
    };

    // 5. Draw the texture centered on the Box2D body coordinates
    // (We pass 0.0f for rotation since your turrets are stationary fortresses)
    DrawTexturePro(m_texture,
                   (Rectangle){ 0.0f, 0.0f, (float)m_texture.width, (float)m_texture.height },
                   dest,
                   origin,
                   0.0f,
                   WHITE);
}

b2BodyId Turret::GetBodyId() const { return m_bodyId; }