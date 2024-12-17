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

            int size = 500.0f;
            float step = size / resolution;

            for (unsigned int z = 0; z <= resolution; ++z)
            {
                for (unsigned int x = 0; x <= resolution; ++x)
                {
                    float xPos = -size / 2.0f + x * step;
                    float zPos = -size / 2.0f + z * step;

                    glm::vec2 pos2D = glm::vec2(xPos, zPos);
                    float height = getHeightAtPosition(pos2D);

                    glm::vec3 pos = glm::vec3(xPos, height, zPos);
                    glm::vec3 col = glm::vec3(1.0f);
                    glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
                    glm::vec2 uv = glm::vec2(x / (float)resolution, z / (float)resolution);

                    vertices.push_back(VertexFormat(pos, col, normal, uv));
                }
            }

            for (unsigned int z = 0; z < resolution; ++z)
            {
                for (unsigned int x = 0; x < resolution; ++x)
                {
                    unsigned int topLeft = z * (resolution + 1) + x;
                    unsigned int topRight = topLeft + 1;
                    unsigned int bottomLeft = (z + 1) * (resolution + 1) + x;
                    unsigned int bottomRight = bottomLeft + 1;

                    indices.push_back(topLeft);
                    indices.push_back(bottomLeft);
                    indices.push_back(topRight);

                    indices.push_back(topRight);
                    indices.push_back(bottomLeft);
                    indices.push_back(bottomRight);
                }
            }

            mesh = new Mesh("Terrain");
            mesh->InitFromData(vertices, indices);
        }

        void Render(Camera *camera)
        {
            shader->Use();
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform));

            GLuint droneHeightLoc = shader->GetUniformLocation("droneHeight");
            if (droneHeightLoc)
                glUniform1f(droneHeightLoc, droneHeight);
            mesh->Render();
        }

        float getHeightAtPosition(const glm::vec2 &position)
        {
            float size = 500.0f;
            float resolution = 100.0f;
            float step = size / resolution;

            float xNormalized = (position.x + size / 2.0f) / step;
            float zNormalized = (position.y + size / 2.0f) / step;
            glm::vec2 noisePosition = position * 0.02f;
            float height = valueNoise(noisePosition) * 20.0f;
            return height;
        }

        bool checkCollision(const glm::vec3 &pos)
        {
            return pos.y <= getHeightAtPosition(glm::vec2(pos.x, pos.z));
        }

        void setDroneHeight(float height)
        {
            droneHeight = height;
        }

    private:
        unsigned int resolution;
        Mesh *mesh;
        glm::mat4 transform = glm::mat4(1.0f);
        Shader *shader;
        std::string name;

        float droneHeight = 0.0f;
    };
}
