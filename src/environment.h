#pragma once
#include "entity.h"
#include <vector>

class Environment 
{
public:
    Environment(b2WorldId worldId, int screenWidth, int screenHeight);
    ~Environment();

    void Draw() const;

private:
    std::vector<Entity> m_groundEntities;
    std::vector<Entity> m_boxEntities;

    Texture2D m_groundTexture;
    Texture2D m_boxTexture;
};