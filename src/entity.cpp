#include "Entity.h"
#include "raymath.h"

void Entity::Draw() const
{
    b2Vec2 p = b2Body_GetWorldPoint(bodyId, { -extent.x, -extent.y });
    b2Rot rotation = b2Body_GetRotation(bodyId);
    float radians = b2Rot_GetAngle(rotation);

    Vector2 ps = { p.x, p.y };
    DrawTextureEx(texture, ps, RAD2DEG * radians, scale, WHITE);
}