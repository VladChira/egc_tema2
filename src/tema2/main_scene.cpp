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

void GenerateTrees(std::vector<glm::mat4> &treeTransforms, Shader *shader, Terrain *terrain)
{
    treeTransforms.clear();

    int resolution = 100;
    int size = 500.0f;
    float step = size / resolution; // Distance between grid points

    // Generate vertices
    for (unsigned int z = 0; z <= resolution; ++z)
    {
        for (unsigned int x = 0; x <= resolution; ++x)
        {
            float xPos = -size / 2.0f + x * step;
            float zPos = -size / 2.0f + z * step;

            glm::vec2 pos2D = glm::vec2(xPos, zPos);

            // Get height using the getHeightAtPosition function
            float height = terrain->getHeightAtPosition(pos2D, size, resolution);

            // Calculate noise value for tree placement
            float noiseValue = valueNoise(glm::vec2(xPos, zPos + 1) * 0.1f);

            // Place a tree if the noise value exceeds the threshold
            if (noiseValue > 0.95f)
            {
                glm::mat4 treeTransform = glm::mat4(1.0f);
                treeTransform = glm::translate(treeTransform, glm::vec3(xPos, height - 13.0f, zPos));
                treeTransform = glm::scale(treeTransform, glm::vec3(20.0f));
                treeTransforms.push_back(treeTransform);
            }
        }
    }
}

void MainScene::GenerateObstacles()
{
    int resolution = 100;
    int size = 500.0f;
    float step = size / resolution; // Distance between grid points

    // Generate vertices
    for (unsigned int z = 0; z <= resolution; ++z)
    {
        for (unsigned int x = 0; x <= resolution; ++x)
        {
            float xPos = -size / 2.0f + x * step;
            float zPos = -size / 2.0f + z * step;

            glm::vec2 pos2D = glm::vec2(xPos, zPos);

            // Get height using the getHeightAtPosition function
            float height = terrain->getHeightAtPosition(pos2D, size, resolution);

            // Calculate noise value for tree placement
            float noiseValue = valueNoise(glm::vec2(xPos, zPos + 1) * 0.1f);

            // Place an obstacle if no tree and if the noise value exceeds the threshold
            if (noiseValue < 0.95f && noiseValue > 0.94f)
            {
                glm::mat4 obstacleTransform = glm::mat4(1.0f);
                Obstacle *obs = new Obstacle("Obstacle " + std::to_string(z) + std::to_string(x), ShaderManager::GetShaderByName("ColorOnly"));
                obs->mesh = obstacleMesh;

                obstacleTransform = glm::translate(obstacleTransform, glm::vec3(xPos, height, zPos));
                obstacleTransform = glm::scale(obstacleTransform, glm::vec3(7.0f));
                obs->transform = obstacleTransform;

                obstacles.push_back(obs);
            }
        }
    }
}

void MainScene::GenerateCheckpoints()
{
    int resolution = 100;
    int size = 500.0f;
    float step = size / resolution; // Distance between grid points

    // Generate vertices
    for (unsigned int z = 0; z <= resolution; ++z)
    {
        for (unsigned int x = 0; x <= resolution; ++x)
        {
            float xPos = -size / 2.0f + x * step;
            float zPos = -size / 2.0f + z * step;

            glm::vec2 pos2D = glm::vec2(xPos, zPos);

            // Get height using the getHeightAtPosition function
            float height = terrain->getHeightAtPosition(pos2D, size, resolution);

            // Calculate noise value for tree placement
            float noiseValue = valueNoise(glm::vec2(xPos, zPos + 1) * 0.1f);

            // Place an obstacle if no tree and if the noise value exceeds the threshold
            if (noiseValue < 0.93f && noiseValue > 0.929f)
            {
                glm::mat4 checkPointTransform = glm::mat4(1.0f);
                Checkpoint *c = new Checkpoint("checkpoint", ShaderManager::GetShaderByName("ColorOnly"));

                checkPointTransform = glm::translate(checkPointTransform, glm::vec3(xPos, height + 12.0f, zPos));
                checkPointTransform = glm::scale(checkPointTransform, glm::vec3(7.0f));
                c->transform = checkPointTransform;

                checkpoints.push_back(c);
            }
        }
    }
}

void MainScene::Init()
{
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    window->SetVSync(true);

    ShaderManager::Initialize(window->props.selfDir);
    ShaderManager::LoadShader("Simple", "MVP.Texture.VS.glsl", "Default.FS.glsl");
    ShaderManager::LoadShader("Color", "MVP.Texture.VS.glsl", "Color.FS.glsl");
    ShaderManager::LoadShader("VertexNormal", "MVP.Texture.VS.glsl", "Normals.FS.glsl");
    ShaderManager::LoadShader("VertexColor", "MVP.Texture.VS.glsl", "VertexColor.FS.glsl");

    ShaderManager::LoadShader("Terrain", "terrain.VS.glsl", "terrain.FS.glsl");
    ShaderManager::LoadShader("ColorOnly", "MVP.VS.glsl", "Color.FS.glsl");

    drone = new Drone("drone", ShaderManager::GetShaderByName("ColorOnly"));
    drone->pos = glm::vec3(0.0f, 3.0f, 0.0f);

    terrain = new Terrain("Ground", ShaderManager::GetShaderByName("Terrain"), 100);
    terrain->ComputeMesh();

    tree = new Tree("Tree 1", ShaderManager::GetShaderByName("ColorOnly"));
    GenerateTrees(treeTransforms, ShaderManager::GetShaderByName("ColorOnly"), terrain);

    obstacleMesh = new Mesh("Obstacle");
    obstacleMesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "props"), "Rock1.obj");
    GenerateObstacles();

    GenerateCheckpoints();

    camera = new gfxc::Camera();
    camera->SetPerspective(90, window->props.aspectRatio, 0.01f, 1000);
    camera->m_transform->SetMoveSpeed(2);
    camera->m_transform->SetWorldPosition(glm::vec3(0, 4.6f, 2.5));
    camera->m_transform->SetWorldRotation(glm::vec3(-15, 0, 0));
    camera->Update();
}

void MainScene::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->props.resolution;
    glViewport(0, 0, resolution.x, resolution.y);
    glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drone->Update(deltaTimeSeconds);

    camera->m_transform->SetWorldPosition(drone->pos + glm::vec3(0.0f, 1.5f, 0.0f));
    camera->m_transform->SetReleativeRotation(glm::vec3(-30.0f, glm::degrees(drone->GetYPR().x), glm::degrees(drone->GetYPR().z)));
    camera->Update();

    drone->Render(camera);
    terrain->Render(camera);

    for (glm::mat4 t : treeTransforms)
        tree->Render(camera, t);

    for (auto obs : obstacles)
        obs->Render(camera);

    glLineWidth(5.0f);
    for (auto checkpoint : checkpoints)
        checkpoint->Render(camera);

    // Shader *s = ShaderManager::GetShaderByName("ColorOnly");
    // s->Use();
    // glUniformMatrix4fv(s->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    // glUniformMatrix4fv(s->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
    // glUniformMatrix4fv(s->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    // GLuint colorLoc = s->GetUniformLocation("color");
    // if (colorLoc)
    //     glUniform3f(colorLoc, 0.0f, 0.0f, 0.0f);
    // obstacles[0]->Render();

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
        // float roll_value = axes[0];
        // if (abs(roll_value) < 0.1)
        //     roll_value = 0.0f;
        // drone->Rotate(0.0f, 0.0f, -roll_value);

        // // Pitch control
        // float pitch_value = axes[1];
        // if (abs(pitch_value) < 0.1)
        //     pitch_value = 0.0f;
        // drone->Rotate(0.0f, pitch_value, 0.0f);

        // float up_value = -axes[4];
        // if (abs(up_value) < 0.1)
        //     up_value = 0.0f;
        // drone->motor1Thrust = drone->hoverThrust + drone->maxThrust * up_value;
        // drone->motor2Thrust = drone->hoverThrust + drone->maxThrust * up_value;
        // drone->motor3Thrust = drone->hoverThrust + drone->maxThrust * up_value;
        // drone->motor4Thrust = drone->hoverThrust + drone->maxThrust * up_value;

        // float yaw_value = axes[3];
        // if (abs(yaw_value) < 0.1)
        //     yaw_value = 0.0f;
        // drone->rot.y += -yaw_value * deltaTime;
    }

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
        drone->ResetRotation();
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