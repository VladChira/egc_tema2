#pragma once

#include "core/world.h"
#include "components/camera.h"
#include "core/gpu/mesh.h"

#include "tema2/primitives/primitives.h"
#include "tema2/terrain.h"
#include "tema2/drone.h"
#include "tema2/tree.h"

namespace tema2
{
    class MainScene : public World
    {
    public:
        MainScene();
        ~MainScene();

        void Init();
        void Update(float deltaTimeSeconds);
        void OnInputUpdate(float deltaTime, int mods) override;

    private:
        glm::vec3 skyColor = glm::vec3(0.529, 0.807, 0.921);
        gfxc::Camera *camera;

        Drone *drone;
        Terrain *terrain;
        Tree *tree; // a single tree mesh drawn multiple times!
        std::vector<glm::mat4> treeTransforms;
    };
}