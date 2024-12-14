#pragma once
#include "tema2/primitives/primitive.h"
#include <vector>

namespace tema2
{
    class Checkpoint
    {
    public:
        Checkpoint(std::string name, Shader *shader) : name(name), shader(shader)
        {
            // Create outer square as an outline
            outerSquare = CreateSquareOutline(2.0f);

            // Create inner square as an outline
            innerSquare = CreateSquareOutline(1.2f);

            // Create vertical bars as lines
            leftBar = CreateLine(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(-1.0f, -3.0f, 0.0f));
            rightBar = CreateLine(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(1.0f, -3.0f, 0.0f));
        }

        void Render(gfxc::Camera *camera, bool red)
        {
            shader->Use();

            // Pass camera matrices
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetProjectionMatrix()));

            // Render outer square
            glm::mat4 outerTransform = transform;
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(outerTransform));
            glUniform3f(shader->GetUniformLocation("color"), 1.0f, 1.0f, 1.0f); // Outer square color (white)
            outerSquare->Render();

            // Render inner square
            glm::mat4 innerTransform = transform;
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(innerTransform));
            if (!red)
                glUniform3f(shader->GetUniformLocation("color"), 0.0f, 1.0f, 0.0f);
            else
                glUniform3f(shader->GetUniformLocation("color"), 1.0f, 0.0f, 0.0f);
            innerSquare->Render();

            // Render left bar
            glm::mat4 leftBarTransform = transform;
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(leftBarTransform));
            glUniform3f(shader->GetUniformLocation("color"), 0.0f, 1.0f, 0.0f); // Bar color (green)
            leftBar->Render();

            // Render right bar
            glm::mat4 rightBarTransform = transform;
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(rightBarTransform));
            glUniform3f(shader->GetUniformLocation("color"), 0.0f, 1.0f, 0.0f); // Bar color (green)
            rightBar->Render();
        }

        ~Checkpoint()
        {
            delete outerSquare;
            delete innerSquare;
            delete leftBar;
            delete rightBar;
        }

        // Update the AABB collision bounds, accounting for scaling and transformation
        void UpdateCollisionBounds()
        {
            // Define the local bounds based on the outer square size and vertical bar length
            float squareHalfSize = 1.0f; // Half the outer square size
            float barHeight = 2.0f;      // Vertical bar length

            glm::vec3 localMin(-squareHalfSize, -barHeight, -0.1f);
            glm::vec3 localMax(squareHalfSize, squareHalfSize, 0.1f);

            // Generate all 8 corners of the AABB in local space
            std::vector<glm::vec3> corners = {
                localMin,
                glm::vec3(localMax.x, localMin.y, localMin.z),
                glm::vec3(localMax.x, localMax.y, localMin.z),
                glm::vec3(localMin.x, localMax.y, localMin.z),
                glm::vec3(localMin.x, localMin.y, localMax.z),
                glm::vec3(localMax.x, localMin.y, localMax.z),
                glm::vec3(localMax.x, localMax.y, localMax.z),
                glm::vec3(localMin.x, localMax.y, localMax.z)};

            // Transform all corners into world space
            for (auto &corner : corners)
            {
                corner = glm::vec3(transform * glm::vec4(corner, 1.0f));
            }

            // Recompute the min and max bounds from the transformed corners
            minBounds = corners[0];
            maxBounds = corners[0];

            for (const auto &corner : corners)
            {
                minBounds = glm::min(minBounds, corner);
                maxBounds = glm::max(maxBounds, corner);
            }
        }

        bool CheckCollision(const glm::vec3 &position)
        {
            UpdateCollisionBounds();
            bool collided = (position.x >= minBounds.x && position.x <= maxBounds.x &&
                             position.y >= minBounds.y && position.y <= maxBounds.y &&
                             position.z >= minBounds.z && position.z <= maxBounds.z);
            previousCollided = collided;
            return collided;
        }

    private:
        // Create an outline of a square
        Mesh *CreateSquareOutline(float size)
        {
            std::vector<VertexFormat> vertices = {
                VertexFormat(glm::vec3(-size / 2.0f, size / 2.0f, 0.0f)), // Top-left
                VertexFormat(glm::vec3(size / 2.0f, size / 2.0f, 0.0f)),  // Top-right
                VertexFormat(glm::vec3(size / 2.0f, -size / 2.0f, 0.0f)), // Bottom-right
                VertexFormat(glm::vec3(-size / 2.0f, -size / 2.0f, 0.0f)) // Bottom-left
            };

            std::vector<unsigned int> indices = {
                0, 1, 1, 2, 2, 3, 3, 0 // Line segments
            };

            Mesh *square = new Mesh("squareOutline");
            square->InitFromData(vertices, indices);
            square->SetDrawMode(GL_LINES);
            return square;
        }

        // Create a vertical line
        Mesh *CreateLine(const glm::vec3 &start, const glm::vec3 &end)
        {
            std::vector<VertexFormat> vertices = {
                VertexFormat(start),
                VertexFormat(end)};

            std::vector<unsigned int> indices = {0, 1};

            Mesh *line = new Mesh("line");
            line->InitFromData(vertices, indices);
            line->SetDrawMode(GL_LINES);
            return line;
        }

    public:
        Mesh *outerSquare;
        Mesh *innerSquare;
        Mesh *leftBar;
        Mesh *rightBar;
        glm::mat4 transform = glm::mat4(1.0f);
        Shader *shader;
        std::string name;

        bool previousCollided = false;

        glm::vec3 minBounds;
        glm::vec3 maxBounds;
    };
}
