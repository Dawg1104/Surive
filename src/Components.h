#pragma once

#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>

#include "EntityManager.h"

struct Position {
    float x, y, z;
};

struct Velocity {
    float x, y, z;
};

struct Transform {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale = glm::vec3(1.0f);
};

struct MeshComponent {
    Mesh* mesh;
};

template<typename ComponentType>
class ComponentArray {
    std::array<ComponentType, MAX_ENTITIES> componentArray;
    std::bitset<MAX_ENTITIES> entityHasComponent;

public:
    void InsertData(Entity entity, ComponentType component) {
        assert(entity < MAX_ENTITIES);
        componentArray[entity] = component;
        entityHasComponent.set(entity);
    }

    void RemoveData(Entity entity) {
        assert(entity < MAX_ENTITIES);
        entityHasComponent.reset(entity);
    }

    ComponentType& GetData(Entity entity) {
        assert(entityHasComponent.test(entity) && "Component not found!");
        return componentArray[entity];
    }

    bool HasComponent(Entity entity) {
        return entityHasComponent.test(entity);
    }
};