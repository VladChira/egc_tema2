#include "tema2/main_scene.h"

#include "core/window/window_object.h"
#include "core/gpu/mesh.h"
#include "core/managers/resource_path.h"

#include <glm/glm.hpp>

#include "tema2/utils/shader_manager.h"

#include "components/camera_input.h"
#include "components/scene_input.h"
#include "components/transform.h"

#include "tema2/utils/value_noise.h"

using namespace tema2;

MainScene::MainScene()
{
}

MainScene::~MainScene()
{
}

void GenerateTrees(std::vector<glm::mat4> &treeTransforms, Shader *shader,
                   int gridSize = 100, float treeThreshold = 0.9f, float noiseScale = 0.1f)
{
    treeTransforms.clear();

    // Large prime numbers for hashing
    const int PRIME1 = 73856093;
    const int PRIME2 = 19349663;

    // Loop over a grid of size gridSize x gridSize
    for (int x = -gridSize / 2; x < gridSize / 2; ++x)
    {
        for (int z = -gridSize / 2; z < gridSize / 2; ++z)
        {
            // Map (x, z) to a 1D value using primes for hashing
            float value_noise_input = (x * PRIME1 + z * PRIME2) * noiseScale;

            // Calculate the 1D noise value for this input
            float noiseValue = value_noise(value_noise_input);

            // Place a tree if the noise value exceeds the threshold
            if (noiseValue > treeThreshold)
            {
                glm::mat4 treeTransform = glm::mat4(1.0f);
                treeTransform = glm::scale(treeTransform, glm::vec3(5.0f));
                treeTransform = glm::translate(treeTransform, glm::vec3((float)x, -0.6f, (float)z));
                treeTransforms.push_back(treeTransform);
            }
        }
    }
}

void MainScene::Init()
{
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    ShaderManager::Initialize(window->props.selfDir);
    ShaderManager::LoadShader("Simple", "MVP.Texture.VS.glsl", "Default.FS.glsl");
    ShaderManager::LoadShader("Color", "MVP.Texture.VS.glsl", "Color.FS.glsl");
    ShaderManager::LoadShader("VertexNormal", "MVP.Texture.VS.glsl", "Normals.FS.glsl");
    ShaderManager::LoadShader("VertexColor", "MVP.Texture.VS.glsl", "VertexColor.FS.glsl");

    ShaderManager::LoadShader("ColorOnly", "MVP.VS.glsl", "Color.FS.glsl");

    drone = new Drone("drone", ShaderManager::GetShaderByName("ColorOnly"));
    drone->pos = glm::vec3(0.0f, 3.0f, 0.0f);

    terrain = new Terrain("Ground", ShaderManager::GetShaderByName("ColorOnly"), 100);
    terrain->SetColor(glm::vec3(0.0f, 1.0f, 0.0f));
    terrain->ComputeMesh();

    tree = new Tree("Tree 1", ShaderManager::GetShaderByName("ColorOnly"));

    GenerateTrees(treeTransforms, ShaderManager::GetShaderByName("ColorOnly"));

    camera = new gfxc::Camera();
    camera->SetPerspective(90, window->props.aspectRatio, 0.01f, 200);
    camera->m_transform->SetMoveSpeed(2);
    camera->m_transform->SetWorldPosition(glm::vec3(0, 4.6f, 2.5));
    camera->m_transform->SetWorldRotation(glm::vec3(-15, 90, 0));
    camera->Update();
}

void MainScene::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->props.resolution;
    glViewport(0, 0, resolution.x, resolution.y);
    glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera->Update();

    drone->Update(deltaTimeSeconds);
    drone->Render(camera);

    terrain->Render(camera);

    for (glm::mat4 t : treeTransforms)
        tree->Render(camera, t);

    // Measure speed
    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    static int nbFrames = 0;
    nbFrames++;
    if (currentTime - lastTime >= 1.0)
    {
        printf("%f ms/frame\n", 1000.0 / double(nbFrames));
        nbFrames = 0;
        lastTime += 1.0;
    }
}

void MainScene::OnInputUpdate(float deltaTime, int mods)
{
    if (glfwJoystickPresent(GLFW_JOYSTICK_1))
    {
        int axesCount;
        const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);

        // Roll control
        float roll_value = axes[0];
        if (abs(roll_value) < 0.08)
            roll_value = 0.0f;
        drone->rot.x += -roll_value * deltaTime;
        drone->rot.x = glm::clamp(drone->rot.x, glm::radians(-20.0f), glm::radians(20.0f));

        // Pitch control
        float pitch_value = axes[1];
        if (abs(pitch_value) < 0.08)
            pitch_value = 0.0f;
        drone->rot.z += -pitch_value * deltaTime;
        drone->rot.z = glm::clamp(drone->rot.z, glm::radians(-20.0f), glm::radians(20.0f));
    }

    if (window->KeyHold(GLFW_KEY_RIGHT))
    {
        drone->rot.x -= deltaTime * 1.0f;
        if (drone->rot.x > glm::radians(20.0f))
            drone->rot.x = glm::radians(20.0f);
        if (drone->rot.x < glm::radians(-20.0f))
            drone->rot.x = glm::radians(-20.0f);
    }

    if (window->KeyHold(GLFW_KEY_LEFT))
    {
        drone->rot.x += deltaTime * 1.0f;
        if (drone->rot.x > glm::radians(20.0f))
            drone->rot.x = glm::radians(20.0f);
        if (drone->rot.x < glm::radians(-20.0f))
            drone->rot.x = glm::radians(-20.0f);
    }

    if (window->KeyHold(GLFW_KEY_UP))
    {
        drone->rot.z += deltaTime * 1.0f;
        if (drone->rot.z > glm::radians(20.0f))
            drone->rot.z = glm::radians(20.0f);
        if (drone->rot.z < glm::radians(-20.0f))
            drone->rot.z = glm::radians(-20.0f);
    }

    if (window->KeyHold(GLFW_KEY_DOWN))
    {
        drone->rot.z -= deltaTime * 1.0f;
        if (drone->rot.z > glm::radians(20.0f))
            drone->rot.z = glm::radians(20.0f);
        if (drone->rot.z < glm::radians(-20.0f))
            drone->rot.z = glm::radians(-20.0f);
    }

    if (window->KeyHold(GLFW_KEY_SPACE))
    {
        drone->rot.x = 0.0f;
        drone->rot.z = 0.0f;
        drone->vel = glm::vec3(0.0f);
    }

    if (window->KeyHold(GLFW_KEY_W))
        camera->MoveForward(deltaTime);

    if (window->KeyHold(GLFW_KEY_S))
        camera->MoveBackward(deltaTime);

    if (window->KeyHold(GLFW_KEY_A))
        camera->MoveLeft(deltaTime);

    if (window->KeyHold(GLFW_KEY_D))
        camera->MoveRight(deltaTime);

    if (window->KeyHold(GLFW_KEY_Q))
        camera->MoveUp(deltaTime);

    if (window->KeyHold(GLFW_KEY_E))
        camera->MoveDown(deltaTime);
}