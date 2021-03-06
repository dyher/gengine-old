#pragma once

#include "primitives.h"
#include "debug.h"

namespace gengine
{
namespace graphics
{

struct ParticleVertex
{
    Vector2
        position,
        extent;
    float
        rotation,
        index,
        life;

    static void enableAttributes()
    {
        glEnableVertexAttribArray(Program::ATTRIBUTE_LOCATION_POSITION);
        glVertexAttribPointer(Program::ATTRIBUTE_LOCATION_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, position));

        glEnableVertexAttribArray(Program::ATTRIBUTE_LOCATION_EXTENT);
        glVertexAttribPointer(Program::ATTRIBUTE_LOCATION_EXTENT, 2, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, extent));

        glEnableVertexAttribArray(Program::ATTRIBUTE_LOCATION_ROTATION);
        glVertexAttribPointer(Program::ATTRIBUTE_LOCATION_ROTATION, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, rotation));

        glEnableVertexAttribArray(Program::ATTRIBUTE_LOCATION_INDEX);
        glVertexAttribPointer(Program::ATTRIBUTE_LOCATION_INDEX, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, index));

        glEnableVertexAttribArray(Program::ATTRIBUTE_LOCATION_LIFE);
        glVertexAttribPointer(Program::ATTRIBUTE_LOCATION_LIFE, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), (void*)offsetof(ParticleVertex, life));
    }
};

}
}
