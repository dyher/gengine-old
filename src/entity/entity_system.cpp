#include "entity_system.h"

#include "primitives.h"
#include "array.h"
#include "script.h"
#include "script_system.h"
#include "debug.h"
#include "vector2.h"
#include "entity.h"
#include "entity_component_sprite.h"
#include "entity_component_camera.h"

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

    for(int ref : refTable)
    {
        lua_rawgeti(state, LUA_REGISTRYINDEX, ref);

        lua_getfield(state, -1, "position");

        script::fillVector2(state, currentTransform.position);

        lua_pop(state, 1);

        lua_getfield(state, -1, "rotation");
        currentTransform.rotation = lua_tonumber(state, -1);
        lua_pop(state, 1);

        lua_getfield(state, -1, "update");
        lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
        lua_pushnumber(state, dt);
        lua_call(state, 2, 0);

        pushTransform(state, currentTransform);

        lua_pop(state, 1);
    }
}

SCRIPT_CLASS_REGISTERER(System)
{
    lua_newtable(state);
    SCRIPT_TABLE_PUSH_CLASS_FUNCTION(System, create);
    lua_setglobal(state,"entity");

    registerComponent<ComponentSprite>(state, "ComponentSprite", "sprite");
    registerComponent<ComponentCamera>(state, "ComponentCamera", "camera");
}

SCRIPT_CLASS_UNREGISTERER(System)
{
    // :todo: remove entities/components
}

SCRIPT_CLASS_FUNCTION(System, create)
{
    lua_newtable(state);

    lua_newtable(state);

    SCRIPT_TABLE_PUSH_NUMBER(x, 0);
    SCRIPT_TABLE_PUSH_NUMBER(y, 0);

    lua_setfield(state, -2, "position");

    SCRIPT_TABLE_PUSH_NUMBER(rotation, 0);

    lua_newtable(state);
    lua_setfield(state, -2, "components");

    lua_pushstring(state, "Unnamed");
    lua_setfield(state, -2, "name");

    lua_rawgeti(state, LUA_REGISTRYINDEX, getMetaTableRef());
    lua_setmetatable(state, -2);

    int ref = luaL_ref(state, LUA_REGISTRYINDEX);

    getInstance().refTable.add(ref);

    lua_rawgeti(state, LUA_REGISTRYINDEX, ref);

    return 1;
}


void System::pushTransform(lua_State * state, const Transform & transform)
{
    lua_getfield(state, -1, "position");
    SCRIPT_TABLE_PUSH_NUMBER(x, transform.position.x);
    SCRIPT_TABLE_PUSH_NUMBER(y, transform.position.y);

    lua_pop(state, 1);

    SCRIPT_TABLE_PUSH_NUMBER(rotation, transform.rotation);
}


}
}
