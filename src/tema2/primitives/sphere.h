#pragma once

#include "tema2/primitives/primitive.h"
#include <cmath>

namespace tema2
{
    class Sphere : public Primitive
    {
    public:
        Sphere(std::string name, Shader *shader, float radius = 1.0f, int stacks = 16, int slices = 32)
            : Primitive(name, shader), radius(radius), stacks(stacks), slices(slices) {}

        virtual void ComputeMesh() override
        {
            std::vector<VertexFormat> vertices;
            std::vector<unsigned int> indices;

            for (int stack = 0; stack <= stacks; ++stack)
            {
                float phi = glm::pi<float>() * stack / stacks;
                for (int slice = 0; slice <= slices; ++slice)
                {
                    float theta = (2.0f * glm::pi<float>() * slice) / slices;
                    float x = radius * sin(phi) * cos(theta);
                    float y = radius * cos(phi);
                    float z = radius * sin(phi) * sin(theta);
                    vertices.push_back(VertexFormat(glm::vec3(x, y, z)));
                }
            }

            for (int stack = 0; stack < stacks; ++stack)
            {
                for (int slice = 0; slice < slices; ++slice)
                {
                    int first = (stack * (slices + 1)) + slice;
                    int second = first + slices + 1;

                    indices.push_back(first);
                    indices.push_back(second);
                    indices.push_back(first + 1);

                    indices.push_back(second);
                    indices.push_back(second + 1);
                    indices.push_back(first + 1);
                }
            }

            mesh = new Mesh(name);
            mesh->InitFromData(vertices, indices);
        }

        virtual void Render(Camera *camera) override
        {
            shader->Use();
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform));
            mesh->Render();
        }

    private:
        float radius;
        int stacks;
        int slices;
    };
}
