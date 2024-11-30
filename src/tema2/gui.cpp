// #include "tema2/gui.h"
// #include <iostream>
// using namespace tema2;

// GUI::GUI(GLFWwindow *window)
// {
//     this->window = window;
//     glfwGetWindowSize(window, &(this->width), &(this->height));

//     sceneBuffer = new FrameBuffer();
//     sceneBuffer->Generate(width, height, 1);
// }

// FrameBuffer *GUI::GetSceneBuffer()
// {
//     return this->sceneBuffer;
// }

// GUI::~GUI()
// {
//     OnShutdown();
// }

// void GUI::Init()
// {
//     ImGui::CreateContext();
//     ImGui::StyleColorsDark();
//     ImGui_ImplGlfw_InitForOpenGL(window, true);
//     ImGui_ImplOpenGL3_Init("#version 130");
// }

// void GUI::FrameStart()
// {
//     glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
//     glClear(GL_COLOR_BUFFER_BIT);

//     glfwGetWindowSize(window, &(this->width), &(this->height));

//     ImGui_ImplOpenGL3_NewFrame();
//     ImGui_ImplGlfw_NewFrame();
//     ImGui::NewFrame();
// }

// void GUI::Update(gfxc::Camera *camera, float deltaTimeSeconds)
// {
//     // std::cout << deltaTimeSeconds << "\n";
//     if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
//     {
//         ImVec2 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right, 0.1f);
//         camera->RotateOY(-mouseDelta.x);
//         camera->RotateOX(-mouseDelta.y);
        
//         ImGui::ResetMouseDragDelta(1);
//     }

//     if (ImGui::IsKeyPressed(ImGuiKey_W))
//         camera->MoveForward(5.0f * deltaTimeSeconds);
//     if (ImGui::IsKeyPressed(ImGuiKey_S))
//         camera->MoveForward(-5.0f * deltaTimeSeconds);
//     if (ImGui::IsKeyPressed(ImGuiKey_A))
//         camera->MoveLeft(5.0f * deltaTimeSeconds);
//     if (ImGui::IsKeyPressed(ImGuiKey_D))
//         camera->MoveRight(5.0f * deltaTimeSeconds);
//     if (ImGui::IsKeyPressed(ImGuiKey_Q))
//         camera->MoveUp(5.0f * deltaTimeSeconds);
//     if (ImGui::IsKeyPressed(ImGuiKey_Z))
//         camera->MoveDown(5.0f * deltaTimeSeconds);
//     camera->Update();

//     ImGui::SetNextWindowPos(ImVec2(0, 0));
//     ImGui::SetNextWindowSize(ImVec2(width + 0.01 * width, height + 0.01 * width));
//     if (ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus))
//     {
//         unsigned int tex = sceneBuffer->GetTextureID(0);
//         ImGui::Image(
//             (ImTextureID)tex,
//             ImVec2(width, height),
//             ImVec2(0, 1),
//             ImVec2(1, 0));
//     }
//     ImGui::End();

//     if (ImGui::Begin("Game Window"))
//     {
//         std::string s(std::to_string(ImGui::GetIO().Framerate));
//         s += " FPS";
//         ImGui::Text(s.c_str());
//     }
//     ImGui::End();
// }

// void GUI::FrameEnd()
// {
//     ImGui::Render();
//     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
// }

// void GUI::OnShutdown()
// {
//     ImGui_ImplOpenGL3_Shutdown();
//     ImGui_ImplGlfw_Shutdown();
//     ImGui::DestroyContext();
// }