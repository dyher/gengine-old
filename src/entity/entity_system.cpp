#include "entity_system.h"

#include "primitives.h"
#include "array.h"
#include "script.h"
#include "script_system.h"
#include "debug.h"
#include "vector2.h"
#include "string.h"
#include "entity.h"
#include "entity_entity.h"
#include "entity_component_sprite.h"
#include "entity_component_camera.h"
#include "entity_component_mouseable.h"
#include "entity_component_animated_sprite.h"
#include "entity_component_sprite_batch.h"
#include "entity_component_physic.h"
#include "entity_component_navigation_agent.h"
#include "entity_component_particle_system.h"
#include "entity_component_quad.h"
#include "entity_component_spriter.h"
#include "entity_component_text.h"
#include "entity_component_spine.h"

namespace gengine
{
namespace entity
{

void System::init()
{
}

void System::finalize()
{
}

void System::update(const float dt)
{
    lua_State * state = script::System::getInstance().getState();
    currentDt = dt;

    entitiesToUpdate = entities;

    for(auto pentity : entitiesToUpdate)
    {
        auto & entity = *pentity;

        int ref = entity.ref;
        lua_rawgeti(state, LUA_REGISTRYINDEX, ref);

        script::get(state, entity.transform);

        if(entity.isInserted())
        {
            entity.update(dt);
        }

        lua_pop(state, 1);
    }

    if(entitiesToRemove.getSize() > 0 && entities.getSize() > 0)
    {
        for(int i = int(entities.getSize()) - 1; i >= 0; --i)
        {
            auto & entity = * entities[i];
            int ref = entity.ref;

            for(uint j = 0; j < entitiesToRemove.getSize(); ++j)
            {
                auto & entityToRemove = * entitiesToRemove[j];
                int refToRemove = entityToRemove.ref;

                if(ref == refToRemove)
                {
                    luaL_unref(state, LUA_REGISTRYINDEX, ref);
                    entities.removeAt(i);
                    entitiesToRemove.removeAt(j);
                    delete & entity;
                    break;
                }
            }
        }
    }
}

SCRIPT_CLASS_REGISTERER(System)
{
    lua_newtable(state);

    SCRIPT_TABLE_PUSH_CLASS_FUNCTION(System, getCount);
    SCRIPT_TABLE_PUSH_CLASS_FUNCTION(System, create);
    SCRIPT_TABLE_PUSH_CLASS_FUNCTION(System, destroy);

    SCRIPT_DO(
        return function(_table)
            _table.__call = function()
                local o = {}
                setmetatable(o, _table)
                return o
            end

            _table.__newindex = function(self,k,v)
                rawset(self,k,v)
            end

            _table.__index = function(_t, _key)
                return rawget(getmetatable(_t),_key)
            end

            setmetatable(_table, _table)
        end
        );

    lua_setfield(state, -2, "registerCustomComponent");

    lua_getglobal(state, "_G");
    lua_pushnil(state);

    while(lua_next(state, -2))
    {
        if(lua_type(state, -1) == LUA_TTABLE)
        {
            const char * name = lua_tostring(state, -2);

            if(!strncmp("Component", name, 9))
            {
                if(!lua_getmetatable(state, -1))
                {
                    lua_getfield(state, -4, "registerCustomComponent");

                    lua_getglobal(state, name);

                    lua_call(state, 1, 0);
                }
                else
                {
                    lua_pop(state, 1);
                }
            }
        }

        lua_pop(state, 1);
    }

    lua_pop(state, 1);

    lua_setfield(state, -2, "entity");

    registerComponent<ComponentSprite>(state, "ComponentSprite");
    registerComponent<ComponentCamera>(state, "ComponentCamera");
    registerComponent<ComponentMouseable>(state, "ComponentMouseable");
    registerComponent<ComponentAnimatedSprite>(state, "ComponentAnimatedSprite");
    registerComponent<ComponentSpriteBatch>(state, "ComponentSpriteBatch");
    registerComponent<ComponentPhysic>(state, "ComponentPhysic");
    registerComponent<ComponentNavigationAgent>(state, "ComponentNavigationAgent");
    registerComponent<ComponentParticleSystem>(state, "ComponentParticleSystem");
    registerComponent<ComponentQuad>(state, "ComponentQuad");
    registerComponent<ComponentSpriter>(state, "ComponentSpriter");
    registerComponent<ComponentText>(state, "ComponentText");
    registerComponent<ComponentSpine>(state, "ComponentSpine");
}

SCRIPT_CLASS_UNREGISTERER(System)
{
    // :todo: remove entities/components
}

SCRIPT_CLASS_FUNCTION(System, create)
{
    lua_newtable(state);

    SCRIPT_DO(
        return vector2()
        );

    lua_setfield(state, -2, "position");

    SCRIPT_DO(
        return vector2(1, 1)
        );

    lua_setfield(state, -2, "scale");

    SCRIPT_TABLE_PUSH_NUMBER(rotation, 0);

    lua_newtable(state);
    lua_setfield(state, -2, "components");

    lua_rawgeti(state, LUA_REGISTRYINDEX, getMetaTableRef());
    lua_setmetatable(state, -2);

    int ref = luaL_ref(state, LUA_REGISTRYINDEX);

    Entity *entity = new Entity();
    entity->ref = ref;

    getInstance().entities.add(entity);

    lua_rawgeti(state, LUA_REGISTRYINDEX, ref);

    lua_pushlightuserdata(state, entity);
    lua_setfield(state, -2, "_e");

    return 1;
}

SCRIPT_CLASS_FUNCTION(System, getCount)
{
    lua_pushnumber(state, getInstance().entities.getSize());

    return 1;
}

SCRIPT_CLASS_FUNCTION(System, destroy)
{
    lua_getfield(state, 1, "destroyed");

    if(lua_isnil(state, -1))
    {
        lua_pop(state, 1);

        lua_getfield(state, 1, "_e");
        auto entity = reinterpret_cast<Entity*>(lua_touserdata(state, -1));
        lua_pop(state, 1);

        if(entity->isInserted())
        {
            entity->remove();
        }

        lua_pushboolean(state, true);
        lua_setfield(state, 1, "destroyed");

        getInstance().entitiesToRemove.add(entity);
        entity->finalize();
    }
    else
    {
        lua_pop(state, 1);
    }

    return 0;
}

}
}
