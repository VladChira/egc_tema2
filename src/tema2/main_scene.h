#pragma once

#include <random>

#include "core/world.h"
#include "components/camera.h"
#include "core/gpu/mesh.h"

#include "tema2/primitives/primitives.h"
#include "tema2/terrain.h"
#include "tema2/drone.h"
#include "tema2/tree.h"
#include "tema2/obstacle.h"
#include "tema2/checkpoint.h"
#include "tema2/utils/timer.h"

#include "components/text_renderer.h"

namespace tema2
{
    class MainScene : public World
    {
    public:
        MainScene() {}
        ~MainScene() {}

        void Init();
        void Update(float deltaTimeSeconds);
        void OnInputUpdate(float deltaTime, int mods) override;

        void GenerateRandomPositions(std::vector<glm::vec2> &positions, int count, float size, std::mt19937 &rng);
        void GenerateCheckpoints();
        void GenerateObstacles();
        void GenerateTrees();

        void ResetScene();
        void RenderMinimap(int w, int h);

    private:
        glm::vec3 skyColor = glm::vec3(0.529, 0.807, 0.921);
        gfxc::Camera *mainCamera;
        gfxc::Camera *minimapCamera;
        gfxc::Camera *arrowCam;

        Drone *drone;
        Terrain *terrain;
        Tree *tree; // a single tree mesh drawn multiple times!
        std::vector<glm::mat4> treeTransforms;

        Mesh *obstacleMesh;
        std::vector<Obstacle *> obstacles;
        std::vector<glm::vec2> existingPositions;
        Arrow *arrow;

        std::vector<Checkpoint *> checkpoints;
        Checkpoint **activeCheckpoint;
        std::vector<int> indices;
        int currentIndex = 0;
        bool completed = false;

        bool game_over = false;

        gfxc::TextRenderer *textRenderer;
        Timer timer;
    };
}