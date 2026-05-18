#include "Environment.h"

Environment::Environment(b2WorldId worldId, int screenWidth, int screenHeight) :
m_groundTexture(LoadTexture("assets/ground.png")),
m_boxTexture(LoadTexture("assets/box.png"))
{
    // 1. Load Textures
    m_groundTexture = LoadTexture("assets/ground.png");
    m_boxTexture = LoadTexture("assets/box.png");

    b2Vec2 groundExtent = {0.5f * m_groundTexture.width, 0.5f * m_groundTexture.height};
    b2Vec2 boxExtent = {0.5f * m_boxTexture.width, 0.5f * m_boxTexture.height};

    b2Polygon groundPolygon = b2MakeBox(groundExtent.x, groundExtent.y);
    b2Polygon boxPolygon = b2MakeBox(boxExtent.x, boxExtent.y);

    // 2. Create Ground Entities
    for (int i = 0; i < 14; ++i) {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.position = {(2.0f * i + 2.0f) * groundExtent.x, screenHeight - groundExtent.y - 100.0f};

        b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2CreatePolygonShape(bodyId, &shapeDef, &groundPolygon);

        m_groundEntities.push_back({bodyId, groundExtent, m_groundTexture, 1.0f});
    }

    // 3. Create Box Entities (Pyramid Formation)
    for (int i = 0; i < 4; ++i) {
        float y = screenHeight - groundExtent.y - 100.0f - (2.5f * i + 2.0f) * boxExtent.y - 20.0f;
        for (int j = i; j < 4; ++j) {
            float x = 0.5f * screenWidth + (3.0f * j - i - 3.0f) * boxExtent.x;

            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = {x, y};

            b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
            b2ShapeDef shapeDef = b2DefaultShapeDef();
            b2CreatePolygonShape(bodyId, &shapeDef, &boxPolygon);

            m_boxEntities.push_back({bodyId, boxExtent, m_boxTexture, 1.0f});
        }
    }
}

Environment::~Environment() {
    // Clean up our specific environment assets
    UnloadTexture(m_groundTexture);
    UnloadTexture(m_boxTexture);
}

void Environment::Draw() const {
    for (const auto &ground: m_groundEntities) ground.Draw();
    for (const auto &box: m_boxEntities) box.Draw();
}
