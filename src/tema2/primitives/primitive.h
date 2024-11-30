#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "core/gpu/mesh.h"
#include "tema2/utils/shader_manager.h"
#include "components/camera.h"

namespace tema2
{
    class Primitive
    {
    public:
        Primitive(std::string name, Shader *shader)
        {
            this->name = name;
            this->shader = shader;
        }
        virtual ~Primitive() {};

        virtual void ComputeMesh() = 0;
        virtual void Render(gfxc::Camera *camera) = 0;

        virtual void Translate(glm::vec3 newPosition)
        {
            transform = glm::translate(transform, newPosition);
        }

        virtual void RotateX(float angle_RAD)
        {
            transform = glm::rotate(transform, angle_RAD, glm::vec3(1.0, 0.0, 0.0));
        }

        virtual void RotateY(float angle_RAD)
        {
            transform = glm::rotate(transform, angle_RAD, glm::vec3(0.0, 1.0, 0.0));
        }

        virtual void RotateZ(float angle_RAD)
        {
            transform = glm::rotate(transform, angle_RAD, glm::vec3(0.0, 0.0, 1.0));
        }

        virtual void Scale(glm::vec3 scaleFactor)
        {
            transform = glm::scale(transform, scaleFactor);
        }

        virtual void SetTransform(glm::mat4 newTransform = glm::mat4(1))
        {
            transform = newTransform;
        }

        virtual void SetColor(glm::vec3 color)
        {
            this->color = color;
        }

        glm::mat4 GetTransform()
        {
            return transform;
        }

    protected:
        std::string name;
        Mesh *mesh;
        Shader *shader;
        glm::vec3 color = glm::vec3(0.0f);
        glm::mat4 transform = glm::mat4(1);
    };
}