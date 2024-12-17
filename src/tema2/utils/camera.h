#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

namespace tema2
{
    class Camera
    {
    public:
        Camera()
        {
            SetWorldPosition(glm::vec3(0.0f, 0.0f, 0.0f));
            SetRelativeRotation(glm::vec3(0.0f, 0.0f, 0.0f));
            fovY = glm::radians(45.0f);
            aspectRatio = 16.0f / 9.0f;
            zNear = 0.1f;
            zFar = 100.0f;
            isPerspective = true;
            Update();
        }

        ~Camera() {}

        void SetWorldPosition(const glm::vec3 &position)
        {
            this->position = position;
        }

        void SetRelativeRotation(const glm::vec3 &rotation)
        {
            this->rotation = rotation;
            UpdateDirections();
        }

        void SetPerspective(float FoVy, float aspectRatio, float zNear, float zFar)
        {
            this->fovY = glm::radians(FoVy);
            this->aspectRatio = aspectRatio;
            this->zNear = zNear;
            this->zFar = zFar;
            isPerspective = true;
            Update();
        }

        void SetOrthographic(float width, float height, float zNear, float zFar)
        {
            orthoWidth = width;
            orthoHeight = height;
            this->zNear = zNear;
            this->zFar = zFar;
            isPerspective = false;
            Update();
        }

        void Update()
        {
            if (isPerspective)
            {
                projectionMatrix = glm::perspective(fovY, aspectRatio, zNear, zFar);
            }
            else
            {
                float hw = orthoWidth / 2.0f;
                float hh = orthoHeight / 2.0f;
                projectionMatrix = glm::ortho(-hw, hw, -hh, hh, zNear, zFar);
            }

            viewMatrix = glm::lookAt(position, position + forward, up);
        }

        glm::mat4 GetViewMatrix() const
        {
            return viewMatrix;
        }

        glm::mat4 GetProjectionMatrix() const
        {
            return projectionMatrix;
        }

    private:
        void UpdateDirections()
        {
            forward.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
            forward.y = sin(glm::radians(rotation.x));
            forward.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
            forward = glm::normalize(forward);

            right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));

            glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), forward);
            right = glm::normalize(glm::vec3(rollMatrix * glm::vec4(right, 0.0f)));
            up = glm::normalize(glm::cross(right, forward));
        }

    public:
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 forward;
        glm::vec3 up;
        glm::vec3 right;

    private:
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;

        float fovY;
        float aspectRatio;
        float zNear, zFar;

        float orthoWidth;
        float orthoHeight;

        bool isPerspective;
    };
}
