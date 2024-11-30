#pragma once

#include <string>

#include "tema2/primitives/primitives.h"

namespace tema2
{
    class Propeller
    {
    public:
        Propeller(std::string name, Shader *shader, float bladeLength = 1.0f, float bladeWidth = 0.1f, float bladeHeight = 0.1f,
                  float hubRadius = 0.2f, float hubHeight = 0.05f)
            : name(name), shader(shader), bladeLength(bladeLength), bladeWidth(bladeWidth), bladeHeight(bladeHeight), hubRadius(hubRadius), hubHeight(hubHeight)
        {
            blade1 = new RectangularPrism(name + "_blade1", shader);
            blade1->ComputeMesh();
            blade1->SetColor(glm::vec3(0.1f));

            blade2 = new RectangularPrism(name + "_blade2", shader);
            blade2->ComputeMesh();
            blade2->SetColor(glm::vec3(0.1f));

            hub = new Cylinder(name + "_hub", shader, hubRadius, hubHeight);
            hub->ComputeMesh();
            // hub->Translate(glm::vec3(0.0f, -hubHeight / 2.5f, 0.0f));
            hub->SetColor(glm::vec3(0.95f));
        }

        ~Propeller()
        {
            delete blade1;
            delete blade2;
            delete hub;
        }

        void Update(float deltaTimeSeconds)
        {
            if (bladesSpinning)
            {
                bladeAngle_RAD += angularVelocity * deltaTimeSeconds;
                if (bladeAngle_RAD >= glm::radians(360.0f))
                    bladeAngle_RAD = 0.0f;

            }
        }

        void SetTransform(glm::mat4 newTransform)
        {
            transform = newTransform;
        }

        void Render(gfxc::Camera *camera)
        {
            // Calculate the final transform of the blade by
            // going through the transformation hierachy
            glm::mat4 blade1LocalTransform = ComputeBlade1LocalTransform();
            glm::mat4 blade1FinalTransform = this->transform * blade1LocalTransform;
            blade1->SetTransform(blade1FinalTransform);
            blade1->Render(camera);

            glm::mat4 blade2LocalTransform = ComputeBlade2LocalTransform();
            glm::mat4 blade2FinalTransform = this->transform * blade2LocalTransform;
            blade2->SetTransform(blade2FinalTransform);
            blade2->Render(camera);

            glm::mat4 hubLocalTransform = ComputeHubLocalTransform();
            glm::mat4 hubFinalTransform = this->transform * hubLocalTransform;
            hub->SetTransform(hubFinalTransform);
            hub->Render(camera);
        }

    private:
        glm::mat4 ComputeBlade1LocalTransform()
        {
            glm::mat4 t = glm::mat4(1.0f);
            t = glm::rotate(t, bladeAngle_RAD, glm::vec3(0, 1, 0));
            t = glm::scale(t, glm::vec3(bladeLength, bladeHeight, bladeWidth));
            t = glm::translate(t, glm::vec3(0.0f, 1.0f, 0.0f));
            return t;
        }

        glm::mat4 ComputeBlade2LocalTransform()
        {
            glm::mat4 t = glm::mat4(1.0f);
            t = glm::rotate(t, glm::radians(90.0f) + bladeAngle_RAD, glm::vec3(0, 1, 0));
            t = glm::scale(t, glm::vec3(bladeLength, bladeHeight, bladeWidth));
            t = glm::translate(t, glm::vec3(0.0f, 1.0f, 0.0f));
            return t;
        }

        glm::mat4 ComputeHubLocalTransform()
        {
            glm::mat4 t = glm::mat4(1.0f);
            t = glm::translate(t, glm::vec3(0.0f, -hubHeight / 2.5f, 0.0f));
            return t;
        }

        std::string name;
        Shader *shader;

        RectangularPrism *blade1;
        RectangularPrism *blade2;
        Cylinder *hub;

        glm::mat4 transform = glm::mat4(1.0f);

        float bladeLength;
        float bladeWidth;
        float bladeHeight;

        float hubRadius;
        float hubHeight;

        bool bladesSpinning = true;
        float bladeAngle_RAD = 0.0f;
        float angularVelocity = 20.0f;
    };
}
