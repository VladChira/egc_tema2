#include "tema2/main_scene.h"
#include "core/window/window_object.h"
#include "core/gpu/mesh.h"
#include "core/managers/resource_path.h"
#include "tema2/utils/shader_manager.h"
#include "tema2/utils/value_noise.h"
#include "components/camera_input.h"
#include "components/scene_input.h"
#include "components/transform.h"
#include <glm/glm.hpp>
#include <algorithm>

namespace tema2
{
    const int TERRAIN_RESOLUTION = 100;
    const float TERRAIN_SIZE = 500.0f;
    const float TREE_SCALE = 20.0f;
    const float OBSTACLE_SCALE = 7.0f;
    const float CHECKPOINT_SCALE = 7.0f;
    const float MIN_DISTANCE_BETWEEN_OBJECTS = 30.0f;

    void MainScene::GenerateRandomPositions(std::vector<glm::vec2> &positions, int count, float size, std::mt19937 &rng)
    {
        std::uniform_real_distribution<float> dist(-size / 2.0f, size / 2.0f);
        positions.clear();

        for (int i = 0; i < count; ++i)
        {
            // Find a new valid randomn position with rejection sampling
            glm::vec2 newPosition;
            bool valid;
            do
            {
                valid = true;
                newPosition = glm::vec2(dist(rng), dist(rng));

                for (const auto &pos : existingPositions)
                {
                    if (glm::distance(newPosition, pos) < MIN_DISTANCE_BETWEEN_OBJECTS)
                    {
                        valid = false;
                        break;
                    }
                }

                for (const auto &pos : positions)
                {
                    if (glm::distance(newPosition, pos) < MIN_DISTANCE_BETWEEN_OBJECTS)
                    {
                        valid = false;
                        break;
                    }
                }
            } while (!valid);

            positions.push_back(newPosition);
            existingPositions.push_back(newPosition);
        }
    }

    void MainScene::GenerateTrees()
    {
        int treeCount = 100;
        std::mt19937 rng(42);
        std::vector<glm::vec2> treePositions;
        GenerateRandomPositions(treePositions, treeCount, TERRAIN_SIZE, rng);

        treeTransforms.clear();
        for (const auto &pos : treePositions)
        {
            float height = terrain->getHeightAtPosition(pos);
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, glm::vec3(pos.x, height - 9.0f, pos.y));
            transform = glm::scale(transform, glm::vec3(TREE_SCALE));
            treeTransforms.push_back(transform);
        }
    }

    void MainScene::GenerateObstacles()
    {
        int obstacleCount = 20;
        std::mt19937 rng(std::random_device{}());
        std::vector<glm::vec2> obstaclePositions;
        GenerateRandomPositions(obstaclePositions, obstacleCount, TERRAIN_SIZE, rng);

        obstacles.clear();
        for (const auto &pos : obstaclePositions)
        {
            float height = terrain->getHeightAtPosition(pos);

            auto *obs = new Obstacle("Obstacle", ShaderManager::GetShaderByName("ColorOnly"));
            obs->mesh = obstacleMesh;

            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, glm::vec3(pos.x, height, pos.y));
            transform = glm::scale(transform, glm::vec3(OBSTACLE_SCALE));
            obs->transform = transform;

            obstacles.push_back(obs);
        }
    }

    void MainScene::GenerateCheckpoints()
    {
        int checkpointCount = 3;
        std::mt19937 rng(std::random_device{}());
        std::vector<glm::vec2> checkpointPositions;
        GenerateRandomPositions(checkpointPositions, checkpointCount, TERRAIN_SIZE, rng);

        checkpoints.clear();
        for (const auto &pos : checkpointPositions)
        {
            float height = terrain->getHeightAtPosition(pos);

            auto *checkpoint = new Checkpoint("Checkpoint", ShaderManager::GetShaderByName("ColorOnly"));
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, glm::vec3(pos.x, height + 12.0f, pos.y));
            transform = glm::scale(transform, glm::vec3(CHECKPOINT_SCALE));
            checkpoint->transform = transform;

            checkpoints.push_back(checkpoint);
        }
    }

    void MainScene::ResetScene()
    {
        game_over = false;
        std::shuffle(indices.begin(), indices.end(), std::mt19937(std::random_device()()));
        activeCheckpoint = &checkpoints[indices[0]];
        currentIndex = 0;

        delete drone;
        drone = new Drone("Drone", ShaderManager::GetShaderByName("ColorOnly"));
        drone->pos = glm::vec3(0.0f, 50.0f, 0.0f);

        timer.reset();
        timer.start(2, 30);
    }

    void MainScene::RenderMinimap(int width, int height)
    {
        const int MINIMAP_WIDTH = 355;
        const int MINIMAP_HEIGHT = 200;
        const int MINIMAP_X = width - MINIMAP_WIDTH - 10;
        const int MINIMAP_Y = height - MINIMAP_HEIGHT - 10;

        glViewport(MINIMAP_X, MINIMAP_Y, MINIMAP_WIDTH, MINIMAP_HEIGHT);
        glClear(GL_DEPTH_BUFFER_BIT);

        terrain->Render(minimapCamera);

        arrow->Render(minimapCamera);

        for (const auto &transform : treeTransforms)
        {
            tree->Render(minimapCamera, transform);
        }

        for (const auto &obs : obstacles)
        {
            obs->Render(minimapCamera);
        }

        drone->RenderIndicator(minimapCamera, glm::vec3(drone->pos.x, 80.0f, drone->pos.z));

        for (const auto &checkpoint : checkpoints)
        {
            checkpoint->Render(minimapCamera,
                               activeCheckpoint && checkpoint == *activeCheckpoint ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f));
        }

        glViewport(MINIMAP_X, MINIMAP_Y - MINIMAP_HEIGHT - 10, MINIMAP_WIDTH, MINIMAP_HEIGHT);

        arrow->Render(arrowCam);

        glViewport(0, 0, width, height);
    }

    void MainScene::Init()
    {
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        window->SetVSync(true);

        ShaderManager::Initialize(window->props.selfDir);
        ShaderManager::LoadShader("Simple", "MVP.Texture.VS.glsl", "Default.FS.glsl");
        ShaderManager::LoadShader("Color", "MVP.Texture.VS.glsl", "Color.FS.glsl");
        ShaderManager::LoadShader("Terrain", "terrain.VS.glsl", "terrain.FS.glsl");
        ShaderManager::LoadShader("ColorOnly", "MVP.VS.glsl", "Color.FS.glsl");

        drone = new Drone("Drone", ShaderManager::GetShaderByName("ColorOnly"));
        drone->pos = glm::vec3(0.0f, 50.0f, 0.0f);

        terrain = new Terrain("Ground", ShaderManager::GetShaderByName("Terrain"), TERRAIN_RESOLUTION);
        terrain->ComputeMesh();

        tree = new Tree("Tree", ShaderManager::GetShaderByName("ColorOnly"));

        obstacleMesh = new Mesh("Obstacle");
        obstacleMesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "props"), "Rock1.obj");

        GenerateTrees();
        GenerateObstacles();
        GenerateCheckpoints();

        indices.resize(checkpoints.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), std::mt19937(std::random_device()()));
        activeCheckpoint = &checkpoints[indices[0]];
        std::cout << checkpoints.size() << "\n";

        arrow = new Arrow("arrow", ShaderManager::GetShaderByName("ColorOnly"));
        arrow->ComputeMesh();
        arrow->SetColor(glm::vec3(0.0f, 0.0f, 1.0f));

        mainCamera = new Camera();
        mainCamera->SetPerspective(90, window->props.aspectRatio, 0.01f, 1000.0f);
        mainCamera->SetWorldPosition(glm::vec3(0.0f, 4.6f, 2.5f));
        mainCamera->Update();

        minimapCamera = new Camera();
        minimapCamera->SetOrthographic(355.0f, 200.0f, 0.01f, 1000.0f);
        minimapCamera->Update();

        arrowCam = new Camera();
        arrowCam->SetOrthographic(355.0f, 200.0f, 0.01f, 1000.0f);
        arrowCam->SetWorldPosition(glm::vec3(0.0f, 100.0f, 0.0f));
        arrowCam->SetRelativeRotation(glm::vec3(-90.0f, 90.0f, 0.0f));
        arrowCam->Update();

        glm::ivec2 resolution = window->GetResolution();
        textRenderer = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);

        textRenderer->Load(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 28);

        timer.reset();
        timer.start(2, 30);
    }

    void MainScene::Update(float deltaTimeSeconds)
    {
        glm::ivec2 resolution = window->props.resolution;
        glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, resolution.x, resolution.y);

        if (game_over || timer.ended())
            ResetScene();

        drone->Update(deltaTimeSeconds);

        if (activeCheckpoint != nullptr)
        {
            glm::vec3 checkpointPos = glm::vec3((*activeCheckpoint)->transform[3]);
            arrow->Update(drone->pos, drone->GetYPR().x, checkpointPos);
        }

        if (terrain->checkCollision(drone->pos))
            game_over = true;
        terrain->setDroneHeight(drone->pos.y);

        for (glm::mat4 t : treeTransforms)
            if (tree->checkCollision(drone->pos, t))
                game_over = true;
        
        for (auto o : obstacles)
            if (o->checkCollision(drone->pos, 15.0f))
                game_over = true;

        mainCamera->SetWorldPosition(drone->pos + glm::vec3(0.0f, 1.5f, 0.0f));
        mainCamera->SetRelativeRotation(glm::vec3(-30.0f, glm::degrees(-drone->GetYPR().x) - 90.0f, glm::degrees(-drone->GetYPR().z)));
        mainCamera->Update();

        minimapCamera->SetWorldPosition(glm::vec3(drone->pos.x, 100.0f, drone->pos.z));
        minimapCamera->SetRelativeRotation(glm::vec3(-90.0f, glm::degrees(drone->GetYPR().x) - 90.0f, 0.0f));
        minimapCamera->Update();

        drone->Render(mainCamera);
        terrain->Render(mainCamera);

        for (glm::mat4 t : treeTransforms)
            tree->Render(mainCamera, t);

        for (auto obs : obstacles)
            obs->Render(mainCamera);

        glLineWidth(5.0f);
        for (auto checkpoint : checkpoints)
        {
            if (completed)
            {
                checkpoint->Render(mainCamera, glm::vec3(0.0f, 1.0f, 1.0f));
                continue;
            }
            if (activeCheckpoint != nullptr && checkpoint == *activeCheckpoint)
            {
                if (!checkpoint->previousCollided && checkpoint->CheckCollision(drone->pos))
                {
                    // Entered through the active gate
                    currentIndex++;
                    std::cout << currentIndex << "\n";
                    if (currentIndex == checkpoints.size())
                    {
                        // We have completed all the gates, stop
                        activeCheckpoint = nullptr;
                        completed = true;
                        timer.stop();
                        break;
                    }

                    activeCheckpoint = &checkpoints[indices[currentIndex]];
                    break;
                }
                else
                    checkpoint->CheckCollision(drone->pos);

                checkpoint->Render(mainCamera, glm::vec3(1.0f, 0.0f, 0.0f));
                continue;
            }
            checkpoint->Render(mainCamera, glm::vec3(0.0f, 1.0f, 0.0f));
        }

        textRenderer->RenderText(timer.getTime(), 0.02 * resolution.x, 0.05 * resolution.y, 2.0f);

        RenderMinimap(resolution.x, resolution.y);
    }

    void MainScene::OnInputUpdate(float deltaTime, int mods)
    {
        // ------------- YAW ---------------------
        if (window->KeyHold(GLFW_KEY_J) || window->KeyHold(GLFW_KEY_L))
        {
            drone->yawing = true;
        }
        else
        {
            drone->yawing = false;
        }
        if (window->KeyHold(GLFW_KEY_J))
        {
            drone->RotateYaw(deltaTime * 1.0f);
        }

        if (window->KeyHold(GLFW_KEY_L))
        {
            drone->RotateYaw(-deltaTime * 1.0f);
        }
        //------------------------------------------

        //-------------------- PITCH -----------------------
        if (window->KeyHold(GLFW_KEY_UP) || window->KeyHold(GLFW_KEY_DOWN))
        {
            drone->pitching = true;
        }
        else
        {
            drone->pitching = false;
        }

        if (window->KeyHold(GLFW_KEY_UP))
        {
            drone->RotatePitch(-deltaTime * 1.0f);
        }

        if (window->KeyHold(GLFW_KEY_DOWN))
        {
            drone->RotatePitch(deltaTime * 1.0f);
        }
        //-------------------------------------------------

        //---------------- ROLL -------------------------------
        if (window->KeyHold(GLFW_KEY_RIGHT) || window->KeyHold(GLFW_KEY_LEFT))
        {
            drone->rolling = true;
        }
        else
        {
            drone->rolling = false;
        }

        if (window->KeyHold(GLFW_KEY_RIGHT))
        {
            drone->RotateRoll(-deltaTime * 1.0f);
        }

        if (window->KeyHold(GLFW_KEY_LEFT))
        {
            drone->RotateRoll(deltaTime * 1.0f);
        }
        //-----------------------------------------------

        if (window->KeyHold(GLFW_KEY_I))
        {
            drone->verticalDirection = 1;
        }

        if (window->KeyHold(GLFW_KEY_K))
        {
            drone->verticalDirection = -1;
        }

        if (!window->KeyHold(GLFW_KEY_I) && !window->KeyHold(GLFW_KEY_K))
        {
            drone->verticalDirection = 0;
        }

        if (window->KeyHold(GLFW_KEY_SPACE))
        {
            ResetScene();
        }
    }
}
