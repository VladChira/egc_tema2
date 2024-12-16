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
                // Shaft (line segment)
                VertexFormat(glm::vec3(0.0f, 0.0f, 0.0f)),
                VertexFormat(glm::vec3(0.0f, 0.5f, 0.0f)),
                VertexFormat(glm::vec3(0.02f, 0.5f, 0.0f)),
                VertexFormat(glm::vec3(0.02f, 0.0f, 0.0f)),

                // Arrowhead (triangle)
                VertexFormat(glm::vec3(-0.02f, 0.5f, 0.0f)),
                VertexFormat(glm::vec3(0.01f, 0.7f, 0.0f)),
                VertexFormat(glm::vec3(0.04f, 0.5f, 0.0f))};

            std::vector<unsigned int> indices = {
                0, 1, 2, 2, 0, 3, 4, 5, 6};

            mesh = new Mesh(name);
            mesh->InitFromData(vertices, indices);
        }

        virtual void Render(gfxc::Camera *camera) override
        {
            shader->Use();
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform));

            GLuint colorLoc = shader->GetUniformLocation("color");
            if (colorLoc)
                glUniform3f(colorLoc, color.r, color.g, color.b);

            mesh->Render();
        }
    };
}
