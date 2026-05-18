#include "Entity.h"
#include "raymath.h"

void Entity::Draw() const
{
    // 1. Get the true center position in world coordinates
    b2Vec2 position = b2Body_GetPosition(bodyId);

    // 2. Get the accurate rotation angle
    b2Rot rotation = b2Body_GetRotation(bodyId);
    float radians = b2Rot_GetAngle(rotation);

    // 3. Define the source rectangle (the whole texture)
    Rectangle sourceRec = {
        0.0f,
        0.0f,
        static_cast<float>(texture.width),
        static_cast<float>(texture.height)
    };

    // 4. Define the destination rectangle (where it draws in the world, matching its scaled extent)
    Rectangle destRec = {
        position.x,
        position.y,
        static_cast<float>(texture.width) * scale,
        static_cast<float>(texture.height) * scale
    };

    // 5. Define the visual pivot point (the exact center of your destination rectangle)
    Vector2 origin = {
        destRec.width * 0.5f,
        destRec.height * 0.5f
    };

    // 6. Draw with matching center coordinates and unified rotation
    DrawTexturePro(texture, sourceRec, destRec, origin, RAD2DEG * radians, WHITE);
}