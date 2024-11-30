// #pragma once
// #include <iostream>
// #include <stdio.h>
// #include <assert.h>
// #include <vector>
// #include <GL/glew.h>
// #include <GLFW/glfw3.h>

// #include "core/gpu/frame_buffer.h"

// #include "imgui.h"
// #include "imgui_internal.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"

// #include "components/camera.h"

// namespace tema2
// {
//     class GUI
//     {
//     public:
//         int image_width, image_height;
//         GLuint image_texture;

//         GUI(GLFWwindow *window);
//         ~GUI();

//         void Init();

//         void FrameStart();

//         void Update(gfxc::Camera *camera, float deltaTimeSeconds);

//         void FrameEnd();

//         void OnShutdown();

//         FrameBuffer *GetSceneBuffer();

//     private:
//         GLFWwindow *window;
//         ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

//         FrameBuffer *sceneBuffer;
//         int width, height;
//     };
// }