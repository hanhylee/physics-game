#include "Player.h"
#include "raymath.h"

Player::Player(b2WorldId worldId, int screenWidth, int screenHeight) 
{
    // 1. Load Player Textures
    m_playerTexture = LoadTexture("assets/AmberBall-200.png");
    m_swordTexture = LoadTexture("assets/sword1.png");

    b2Vec2 playerExtent = { 0.25f * m_playerTexture.width, 0.25f * m_playerTexture.height };
    b2Vec2 swordExtent = { 0.5f * m_swordTexture.width, 0.5f * m_swordTexture.height };

    b2Polygon playerPolygon = b2MakeBox(playerExtent.x, playerExtent.y);
    b2Polygon swordPolygon = b2MakeBox(swordExtent.x, swordExtent.y);

    // 2. Create Player Entity
    // Using a static drop-in height since the player doesn't know how tall the ground is anymore
    float spawnX = 0.5f * screenWidth - 3.0f * playerExtent.x;
    float spawnY = screenHeight - 300.0f; 

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.angularDamping = 20.0f;
    bodyDef.position = { spawnX, spawnY };
    
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 15.0f;
    b2CreatePolygonShape(bodyId, &shapeDef, &playerPolygon);

    m_playerEntities.push_back({ bodyId, playerExtent, m_playerTexture, 0.5f });

    // 3. Attach Weapon
    m_weaponEntity = AttachWeapon(worldId, m_playerEntities[0].bodyId, m_swordTexture, swordExtent, swordPolygon);
}

Player::~Player() 
{
    // Clean up player assets
    UnloadTexture(m_playerTexture);
    UnloadTexture(m_swordTexture);
}

Entity Player::AttachWeapon(b2WorldId worldId, b2BodyId playerId, Texture2D texture, b2Vec2 extent, b2Polygon polygon)
{
    Entity weapon = {};
    weapon.texture = texture;
    weapon.extent = extent;
    weapon.scale = 4.0f;

    b2Vec2 playerExtent = m_playerEntities[0].extent;
    b2Vec2 playerPos = b2Body_GetPosition(playerId);

    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.gravityScale = 3.0f;
    bodyDef.position = { playerPos.x + playerExtent.x + extent.x, playerPos.y };

    weapon.bodyId = b2CreateBody(worldId, &bodyDef);

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 10.0f;
    shapeDef.material.friction = 0.4f;
    shapeDef.material.restitution = 0.1f;
    b2CreatePolygonShape(weapon.bodyId, &shapeDef, &polygon);

    b2RevoluteJointDef jointDef = b2DefaultRevoluteJointDef();
    jointDef.base.bodyIdA = playerId;
    jointDef.base.bodyIdB = weapon.bodyId;
    jointDef.base.localFrameA.p = { 0.0f , 0.0f };
    jointDef.base.localFrameB.p = {extent.x / 1.0f, extent.y * 7.5f};
    b2CreateRevoluteJoint(worldId, &jointDef);

    return weapon;
}

void Player::FollowCursor() 
{
    if (m_playerEntities.empty()) return;

    Vector2 mouseDelta = GetMouseDelta();
    b2BodyId playerId = m_playerEntities[0].bodyId;
    b2Vec2 cursorPullForce = {
        mouseDelta.x,
        mouseDelta.y
    };

    float mass = b2Body_GetMass(playerId);
    float strength = mass * LENGTH_UNITS_PER_METER * 1.1f;
    b2Vec2 dir = b2Normalize(cursorPullForce);
    b2Vec2 force = { dir.x * strength, dir.y * strength };

    b2Body_ApplyLinearImpulseToCenter(playerId, force, true);
}

void Player::Update(float deltaTime) 
{
    FollowCursor();
    // Add jumping, dashing, or health checks here later
}

void Player::Draw() const 
{
    for (const auto& player : m_playerEntities) player.Draw();
    m_weaponEntity.Draw();
}

b2Vec2 Player::GetPosition() const {
    if (m_playerEntities.empty()) return { 0.0f, 0.0f };

    // Box2D v3 function to get the current world position
    return b2Body_GetPosition(m_playerEntities[0].bodyId);
}
