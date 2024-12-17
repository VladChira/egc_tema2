#pragma once

#include "tema2/primitives/primitive.h"
#include <cmath>

namespace tema2
{
    class Arrow : public Primitive
    {
    public:
        Arrow(std::string name, Shader *shader)
            : Primitive(name, shader) {}

        virtual void ComputeMesh() override
        {
            std::vector<VertexFormat> vertices = {
                VertexFormat(glm::vec3(0.0f, 0.0f, 0.0f)),
                VertexFormat(glm::vec3(0.0f, 0.0f, 0.2f)),
                VertexFormat(glm::vec3(0.02f, 0.0f, 0.2f)),
                VertexFormat(glm::vec3(0.02f, 0.0f, 0.0f)),

                VertexFormat(glm::vec3(-0.02f, 0.0f, 0.2f)),
                VertexFormat(glm::vec3(0.01f, 0.0f, 0.3f)),
                VertexFormat(glm::vec3(0.04f, 0.0f, 0.2f))};

            std::vector<unsigned int> indices = {
                0, 1, 2, 2, 0, 3, 4, 5, 6};

            mesh = new Mesh(name);
            mesh->InitFromData(vertices, indices);
        }

        void Update(glm::vec3 dronePosition, float droneYaw, glm::vec3 targetPosition)
        {
            glm::mat4 t = glm::mat4(1.0f);
            glm::vec3 direction = glm::normalize(glm::vec3(targetPosition.x - dronePosition.x, 0.0f, targetPosition.z - dronePosition.z));

            float targetAngle = atan2(direction.x, direction.z);
            float adjustedAngle = targetAngle - droneYaw;

            t = glm::scale(t, glm::vec3(300.0f, 1.0f, 300.0f));
            t = glm::rotate(t, adjustedAngle, glm::vec3(0.0f, 1.0f, 0.0f));

            this->transform = t;
        }

        virtual void Render(gfxc::Camera *camera) override
        {
            shader->Use();
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform));

            GLuint colorLoc = shader->GetUniformLocation("color");
            if (colorLoc)
                glUniform3f(colorLoc, color.r, color.g, color.b);

            mesh->Render();
        }
    };
}
