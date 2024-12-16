#pragma once

#include "tema2/primitives/primitive.h"
#include <cmath>

namespace tema2
{
    class Cone : public Primitive
    {
    public:
        Cone(std::string name, Shader *shader, float radius = 0.8f, float height = 1.5f, int segments = 32)
            : Primitive(name, shader), radius(radius), height(height), segments(segments) {}

        virtual void ComputeMesh() override
        {
            std::vector<VertexFormat> vertices;
            std::vector<unsigned int> indices;

            vertices.push_back(VertexFormat(glm::vec3(0.0f, 0.0f, 0.0f)));

            for (int i = 0; i < segments; ++i)
            {
                float angle = (2.0f * glm::pi<float>() * i) / segments;
                float x = radius * cos(angle);
                float z = radius * sin(angle);
                vertices.push_back(VertexFormat(glm::vec3(x, 0.0f, z)));
            }

            vertices.push_back(VertexFormat(glm::vec3(0.0f, height, 0.0f)));
            int apexIndex = segments + 1;

            for (int i = 1; i <= segments; ++i)
            {
                int next = (i % segments) + 1;
                indices.push_back(0);
                indices.push_back(i);
                indices.push_back(next);
            }

            for (int i = 1; i <= segments; ++i)
            {
                int next = (i % segments) + 1;
                indices.push_back(i);
                indices.push_back(next);
                indices.push_back(apexIndex);
            }

            mesh = new Mesh(name);
            mesh->InitFromData(vertices, indices);
        }

        bool checkCollision(const glm::vec3 &point, const glm::mat4 &transform)
        {
            glm::mat4 inverseTransform = glm::inverse(transform);

            // Place the point into cone local coordinate system by inverting its transform
            glm::vec3 localPoint = glm::vec3(inverseTransform * glm::vec4(point, 1.0f));

            if (localPoint.y < 0.0f || localPoint.y > height)
                return false;

            float xradius = (radius * (height - localPoint.y)) / height;
            float distanceFromAxis = std::sqrt(localPoint.x * localPoint.x + localPoint.z * localPoint.z);
            return distanceFromAxis <= xradius;
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
        float radius;
        float height;
        int segments;
    };
}
