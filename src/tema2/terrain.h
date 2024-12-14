#pragma once
#include "core/gpu/mesh.h"
#include "tema2/utils/value_noise.h"
namespace tema2
{
    class Terrain
    {
    public:
        Terrain(std::string name, Shader *shader, unsigned int resolution)
            : resolution(resolution), shader(shader), name(name) {}

        void ComputeMesh()
        {
            std::vector<VertexFormat> vertices;
            std::vector<unsigned int> indices;

            // Calculate step size
            int size = 500.0f;
            float step = size / resolution; // Distance between grid points

            // Generate vertices
            for (unsigned int z = 0; z <= resolution; ++z)
            {
                for (unsigned int x = 0; x <= resolution; ++x)
                {
                    float xPos = -size / 2.0f + x * step; // Centered around (0,0)
                    float zPos = -size / 2.0f + z * step;

                    glm::vec2 pos2D = glm::vec2(xPos, zPos);

                    // Get height using the getHeightAtPosition function
                    float height = getHeightAtPosition(pos2D, size, resolution);

                    glm::vec3 pos = glm::vec3(xPos, height, zPos);
                    glm::vec3 col = glm::vec3(1.0f);
                    glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
                    glm::vec2 uv = glm::vec2(x / (float)resolution, z / (float)resolution);

                    vertices.push_back(VertexFormat(pos, col, normal, uv));
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
            mesh = new Mesh("Terrain");
            mesh->InitFromData(vertices, indices);
        }

        void Render(gfxc::Camera *camera)
        {
            shader->Use();
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform));

            glUniform1f(glGetUniformLocation(shader->GetProgramID(), "uTerrainScale"), 20.0f);   // adjust as needed
            glUniform1f(glGetUniformLocation(shader->GetProgramID(), "uNoiseFrequency"), 10.0f); // frequency of noise sampling
            glUniform1f(glGetUniformLocation(shader->GetProgramID(), "uMaxHeight"), 20.0f);

            mesh->Render();
        }

        float getHeightAtPosition(const glm::vec2 &position, float size, unsigned int resolution)
        {
            // Calculate step size
            float step = size / resolution;

            // Map the position into the range used for the grid
            float xNormalized = (position.x + size / 2.0f) / step;
            float zNormalized = (position.y + size / 2.0f) / step;

            // Calculate the value noise position
            glm::vec2 noisePosition = position * 0.02f;

            // Get the height using the valueNoise function
            float height = valueNoise(noisePosition) * 20.0f;

            return height;
        }

    private:
        unsigned int resolution; // Number of subdivisions along one axis
        Mesh *mesh;
        glm::mat4 transform = glm::mat4(1.0f);
        Shader *shader;
        std::string name;
    };
}
