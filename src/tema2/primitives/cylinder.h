#pragma once

#include "tema2/primitives/primitive.h"
#include <cmath>

namespace tema2
{
    class Cylinder : public Primitive
    {
    public:
        Cylinder(std::string name, Shader *shader, float radius = 1.0f, float height = 2.0f, int segments = 32)
            : Primitive(name, shader), radius(radius), height(height), segments(segments) {}

        virtual void ComputeMesh() override
        {
            std::vector<VertexFormat> vertices;
            std::vector<unsigned int> indices;

            // Bottom center vertex
            vertices.push_back(VertexFormat(glm::vec3(0.0f, 0.0f, 0.0f)));

            // Top center vertex
            //

            // Circle vertices for bottom
            for (int i = 0; i <= segments; ++i)
            {
                float angle = (2.0f * glm::pi<float>() * i) / segments;
                float x = radius * cos(angle);
                float z = radius * sin(angle);
                vertices.push_back(VertexFormat(glm::vec3(x, 0.0f, z)));
            }

            // Indices for the bottom circle
            for (int i = 1; i < segments - 1; ++i)
            {
                indices.push_back(0);
                indices.push_back(i);
                indices.push_back(i + 1);
            }
            indices.push_back(0);
            indices.push_back(segments - 1);
            indices.push_back(1);

            int offset = vertices.size();

            vertices.push_back(VertexFormat(glm::vec3(0.0f, height, 0.0f)));
            // Circle vertices for top
            for (int i = 0; i <= segments; ++i)
            {
                float angle = (2.0f * glm::pi<float>() * i) / segments;
                float x = radius * cos(angle);
                float z = radius * sin(angle);
                vertices.push_back(VertexFormat(glm::vec3(x, height, z)));
            }

            // Indices for the top circle
            for (int i = 1; i < segments - 1; ++i)
            {
                indices.push_back(offset);
                indices.push_back(i + offset);
                indices.push_back(i + offset + 1);
            }
            indices.push_back(offset);
            indices.push_back(offset + segments - 1);
            indices.push_back(1 + offset);

            // Indices for the sides
            for (int i = 1; i < segments - 1; ++i)
            {
                // Triangle 1
                indices.push_back(i);
                indices.push_back(offset + i);
                indices.push_back(i + 1);

                // Triangle 2
                indices.push_back(i + 1);
                indices.push_back(offset + i);
                indices.push_back(offset + i + 1);
            }
            // Triangle 1
            indices.push_back(segments - 2);
            indices.push_back(segments - 2 + offset);
            indices.push_back(segments - 1);

            // Triangle 2
            indices.push_back(segments - 1);
            indices.push_back(segments - 2 + offset);
            indices.push_back(segments - 1 + offset);

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

    private:
        float radius; // Radius of the base
        float height; // Height of the cylinder
        int segments; // Number of segments for the base circle
    };
}
