#pragma once

#include "tema2/primitives/primitive.h"

namespace tema2
{
    class RectangularPrism : public Primitive
    {
    public:
        RectangularPrism(std::string name, Shader *shader) : Primitive(name, shader) {}

        virtual void ComputeMesh() override
        {
            std::vector<VertexFormat> vertices;
            std::vector<unsigned int> indices;

            glm::vec3 corners[8] = {
                {-0.5f, -0.5f, 0.5f}, {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}};

            for (const auto &corner : corners)
                vertices.push_back(VertexFormat(corner));

            indices = {
                0, 1, 2, 2, 3, 0,
                4, 5, 6, 6, 7, 4,
                0, 3, 5, 5, 4, 0,
                3, 2, 6, 6, 5, 3,
                2, 1, 7, 7, 6, 2,
                1, 0, 4, 4, 7, 1};

            mesh = new Mesh(name);
            mesh->InitFromData(vertices, indices);
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

        virtual void Render(gfxc::Camera *camera, glm::mat4 newTransform)
        {
            shader->Use();
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(newTransform));

            GLuint colorLoc = shader->GetUniformLocation("color");
            if (colorLoc)
                glUniform3f(colorLoc, color.r, color.g, color.b);
            mesh->Render();
        }
    };
}
