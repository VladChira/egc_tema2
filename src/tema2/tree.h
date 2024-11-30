#pragma once

#include <string>

#include "tema2/primitives/primitives.h"

namespace tema2
{
    class Tree
    {
    public:
        Tree(std::string name, Shader *shader, float trunkRadius = 0.1f, float trunkHeight = 1.0f, 
             float foliageRadius = 0.6f, float foliageHeight = 0.5f)
            : name(name), shader(shader), trunkRadius(trunkRadius), trunkHeight(trunkHeight),
              foliageRadius(foliageRadius), foliageHeight(foliageHeight)
        {
            trunk = new Cylinder(name + "_trunk", shader, trunkRadius, trunkHeight);
            trunk->ComputeMesh();
            trunk->SetColor(glm::vec3(0.55f, 0.27f, 0.07f)); // Brown color

            foliageBottom = new Cone(name + "_foliageBottom", shader, foliageRadius, foliageHeight);
            foliageBottom->ComputeMesh();
            foliageBottom->SetColor(glm::vec3(0.0f, 0.6f, 0.0f)); // Green color

            foliageTop = new Cone(name + "_foliageTop", shader, foliageRadius * 0.8f, foliageHeight);
            foliageTop->ComputeMesh();
            foliageTop->SetColor(glm::vec3(0.0f, 0.8f, 0.0f)); // Lighter green color
        }

        ~Tree()
        {
            delete trunk;
            delete foliageBottom;
            delete foliageTop;
        }

        void Render(gfxc::Camera *camera, glm::mat4 transform = glm::mat4(1.0f))
        {
            glm::mat4 trunkFinalTransform = transform * ComputeTrunkLocalTransform();
            trunk->SetTransform(trunkFinalTransform);
            trunk->Render(camera);

            glm::mat4 foliageBottomFinalTransform = transform * ComputeFoliageBottomLocalTransform();
            foliageBottom->SetTransform(foliageBottomFinalTransform);
            foliageBottom->Render(camera);

            glm::mat4 foliageTopFinalTransform = transform * ComputeFoliageTopLocalTransform();
            foliageTop->SetTransform(foliageTopFinalTransform);
            foliageTop->Render(camera);
        }

    private:
        std::string name;
        Shader *shader;

        Cylinder *trunk;
        Cone *foliageBottom;
        Cone *foliageTop;

        float trunkRadius;
        float trunkHeight;

        float foliageRadius;
        float foliageHeight;

        glm::mat4 ComputeTrunkLocalTransform()
        {
            // Trunk is at the base of the tree
            return glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, trunkHeight / 2.0f, 0.0f));
        }

        glm::mat4 ComputeFoliageBottomLocalTransform()
        {
            // Foliage starts above the trunk
            return glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, trunkHeight + foliageHeight / 2.0f, 0.0f));
        }

        glm::mat4 ComputeFoliageTopLocalTransform()
        {
            // Top foliage is stacked above the bottom foliage
            return glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, trunkHeight + foliageHeight * 1.3f, 0.0f));
        }
    };
}