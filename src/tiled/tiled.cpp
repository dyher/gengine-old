#include "tiled.h"

#include "debug.h"

namespace gengine
{
namespace tiled
{

SCRIPT_REGISTERER()
{
    lua_newtable(state);

    SCRIPT_DO(
        return function(filename, offset)
            print("[gengine] tiled: Loading " .. filename .. "...")
            local result = {}
            local path = filename:match("(.*/)")
            local file = dofile(filename)
            local indexToTileSet = {}
            local indexToTile = {}

            local function split(str, sep)
               local result = {}
               local regex = string.format("([^%s]+)", sep)
               for line,_ in str:gmatch(regex) do
                  table.insert(result, line)
               end
               return result
            end

            offset = offset or vector2(0, 0)

            if file then
                for k, ts in ipairs(file.tilesets) do
                    if ts.image then
                        local texture = gengine.graphics.texture.create(path .. ts.image)
                        ts.atlas = gengine.graphics.atlas.create(ts.name, texture, { width=ts.tilewidth, height=ts.tileheight, spacing=ts.spacing, margin=ts.margin })

                        for i=1,ts.tilecount do
                            table.insert(indexToTileSet, k)
                            indexToTile[i + ts.firstgid - 1] = {properties={}, _components={}, width=ts.tilewidth, height=ts.tileheight}
                        end
                    else
                        for _,__ in ipairs(ts.tiles) do
                            table.insert(indexToTileSet, false)
                        end
                    end

                    if ts.tiles then
                        for _, tile in ipairs(ts.tiles) do
                            indexToTile[tile.id + ts.firstgid] = tile
                            tile.properties = tile.properties or {}
                            tile.width = tile.width or ts.tilewidth
                            tile.height = tile.height or ts.tileheight
                            if tile.properties.component then
                                tile._components = split(tile.properties.component, ",")
                                tile.properties.component = nil
                            else
                                tile._components = {}
                            end
                            if tile.image then
                                tile.texture = gengine.graphics.texture.create(path .. tile.image)
                            end
                        end
                    end
                end

                for l, layer in ipairs(file.layers) do

                    if layer.properties.component then
                        layer._components = split(layer.properties.component, ",")
                        layer.properties.component = nil
                    else
                        layer._components = {}
                    end

                    if layer.data and layer.encoding == "lua" then
                        for k, v in ipairs(layer.data) do
                            if v ~= 0 then
                                local e = gengine.entity.create()
                                local x = (k-1) % layer.width
                                local y = layer.height - math.floor((k-1) / layer.width)
                                local tile = indexToTile[v]
                                local properties = {}
                                local components = {}

                                for _, p in pairs(tile.properties) do properties[_] = p end
                                for _, p in pairs(layer.properties) do properties[_] = p end
                                for _, c in ipairs(tile._components) do components[c] = true end
                                for _, c in ipairs(layer._components) do components[c] = true end

                                for component, _ in pairs(components) do
                                    if component == "Sprite" then
                                        if not tile.texture then
                                            local ts = file.tilesets[indexToTileSet[v]]
                                            assert(ts, "Error in tiled")
                                            e:addComponent(
                                                ComponentSprite(),
                                                {
                                                    atlas = ts.atlas,
                                                    atlasItem = v - ts.firstgid,
                                                    extent = vector2(tile.width, tile.height),
                                                    layer = l
                                                },
                                                component:lower()
                                                )
                                        else
                                            e:addComponent(
                                                ComponentSprite(),
                                                {
                                                    texture = tile.texture,
                                                    extent = vector2(tile.width, tile.height),
                                                    layer = l
                                                },
                                                component:lower()
                                                )
                                        end
                                    elseif component == "Physic" then
                                        e:addComponent(
                                            ComponentPhysic(),
                                            {
                                                extent = vector2(tile.width, tile.height),
                                                type = properties.type
                                            },
                                            component:lower()
                                            )
                                    else
                                        local constructor = _G["Component"..component]
                                        if constructor then
                                            e:addComponent(
                                                constructor(),
                                                properties,
                                                component:lower()
                                                )
                                        else
                                            print("[gengine] tiled : Unknown component " .. component)
                                        end
                                    end
                                end

                                e.position.x = (x-0.5) * file.tilewidth + tile.width/2 + offset.x
                                e.position.y = (y-0.5) * file.tileheight + tile.height/2 + offset.y

                                table.insert(result, e)
                            end
                        end
                    end

                    if layer.objects then
                        for o, object in ipairs(layer.objects) do
                            local properties = {}
                            for _, p in pairs(object.properties) do properties[_] = p end
                            for _, p in pairs(layer.properties) do properties[_] = p end

                            object.y = file.tileheight * file.height - object.y

                            if object.polyline then
                                for _, v in ipairs(object.polyline) do
                                    v.y = - v.y
                                end
                            end

                            if properties["function"] then
                                local f = loadstring("return " .. properties["function"])()
                                f(object, properties)
                            end
                        end
                    end
                end
            end

            return result
        end
    );
    lua_setfield(state, -2, "createEntities");

    lua_setfield(state, -2, "tiled");
}

}
}
