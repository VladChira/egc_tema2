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

        void Render(Camera *camera)
        {
            shader->Use();
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform));

            GLuint colorLoc = shader->GetUniformLocation("color");
            if (colorLoc)
                glUniform3f(colorLoc, 0.513f, 0.521f, 0.474f);
            mesh->Render();
        }

        bool checkCollision(const glm::vec3 &point, float radius) const
        {
            glm::vec3 obstaclePosition = glm::vec3(transform[3][0], transform[3][1], transform[3][2]);
            float distance = glm::distance(point, obstaclePosition);
            return distance <= radius;
        }

        Mesh *mesh;
        glm::mat4 transform = glm::mat4(1.0f);
        std::string name;
        Shader *shader;
    };
}