#pragma once
#include "tema2/primitives/primitive.h"

namespace tema2
{
    class Obstacle
    {
    public:
        Obstacle(std::string name, Shader *shader) : name(name), shader(shader)
        {
        }

        void Render(gfxc::Camera *camera)
        {
            shader->Use();
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform));

            GLuint colorLoc = shader->GetUniformLocation("color");
            if (colorLoc)
                glUniform3f(colorLoc, 0.513f, 0.521f, 0.474f);
            mesh->Render();

            mesh->Render();
        }
        Mesh *mesh;
        glm::mat4 transform = glm::mat4(1.0f);
        Shader *shader;
        std::string name;
    };
}