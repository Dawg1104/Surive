#pragma once
#include "Components.h"

class MovementSystem {
    ComponentArray<Position>& positions;
    ComponentArray<Velocity>& velocities;

public:
    MovementSystem(ComponentArray<Position>& pos, ComponentArray<Velocity>& vel)
        : positions(pos), velocities(vel) {
    }

    void Update(float dt) {
        for (Entity e = 0; e < MAX_ENTITIES; ++e) {
            if (positions.HasComponent(e) && velocities.HasComponent(e)) {
                auto& pos = positions.GetData(e);
                auto& vel = velocities.GetData(e);
                pos.x += vel.x * dt;
                pos.y += vel.y * dt;
                pos.z += vel.z * dt;
            }
        }
    }
};

class RenderSystem {
    ComponentArray<Transform>& transforms;
    ComponentArray<MeshComponent>& meshes;

public:
    RenderSystem(ComponentArray<Transform>& t, ComponentArray<MeshComponent>& m)
        : transforms(t), meshes(m) {
    }

    void Render(Shader& shader) {
        for (Entity e = 0; e < MAX_ENTITIES; ++e) {
            if (transforms.HasComponent(e) && meshes.HasComponent(e)) {
                auto& transform = transforms.GetData(e);
                auto& meshComp = meshes.GetData(e);

                glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.position);
                model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
                model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
                model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));
                model = glm::scale(model, transform.scale);

                shader.setMat4("model", model);
                meshComp.mesh->Draw(shader);
            }
        }
    }
};