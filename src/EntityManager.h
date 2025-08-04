#pragma once
#include <queue>
#include <bitset>
#include <cassert>
#include <cstdint>

using Entity = std::uint32_t;
constexpr Entity MAX_ENTITIES = 5000;

class EntityManager {
    std::queue<Entity> availableEntities;
    std::bitset<MAX_ENTITIES> activeEntities;
    Entity livingEntityCount = 0;

public:
    EntityManager() {
        for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
            availableEntities.push(entity);
    }

    Entity CreateEntity() {
        assert(livingEntityCount < MAX_ENTITIES && "Too many entities!");
        Entity id = availableEntities.front();
        availableEntities.pop();
        activeEntities.set(id);
        livingEntityCount++;
        return id;
    }

    void DestroyEntity(Entity entity) {
        assert(activeEntities.test(entity) && "Entity does not exist!");
        activeEntities.reset(entity);
        availableEntities.push(entity);
        livingEntityCount--;
    }
};