#pragma once

#include "tema2/primitives/primitive.h"

namespace tema2
{
    class Terrain : public Primitive
    {
    public:
        Terrain(std::string name, Shader *shader, unsigned int resolution)
            : Primitive(name, shader), resolution(resolution) {}

        virtual void ComputeMesh() override
        {
            std::vector<VertexFormat> vertices;
            std::vector<unsigned int> indices;

            // Calculate step size
            int size = 1000.0f;
            float step = size / resolution; // Distance between grid points

            // Generate vertices
            for (unsigned int z = 0; z <= resolution; ++z)
            {
                for (unsigned int x = 0; x <= resolution; ++x)
                {
                    float xPos = -size / 2.0f + x * step; // Centered around (0,0)
                    float zPos = -size / 2.0f + z * step;
                    vertices.emplace_back(glm::vec3(xPos, 0.0f, zPos)); // Flat terrain (y = 0)
                }
            }

            // Generate indices
            for (unsigned int z = 0; z < resolution; ++z)
            {
                for (unsigned int x = 0; x < resolution; ++x)
                {
                    unsigned int topLeft = z * (resolution + 1) + x;
                    unsigned int topRight = topLeft + 1;
                    unsigned int bottomLeft = (z + 1) * (resolution + 1) + x;
                    unsigned int bottomRight = bottomLeft + 1;

                    // First triangle of the grid cell
                    indices.push_back(topLeft);
                    indices.push_back(bottomLeft);
                    indices.push_back(topRight);

                    // Second triangle of the grid cell
                    indices.push_back(topRight);
                    indices.push_back(bottomLeft);
                    indices.push_back(bottomRight);
                }
            }

            // Create the mesh
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
        unsigned int resolution; // Number of subdivisions along one axis
    };
}
