#pragma once
#include "Shader.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tuple>

class Planet {
	public:
		// Calculate the center of mass of the two suns using the mass ratio
		float massratio = 3;
		float totalMassRatio = massratio + 1.0f;
        glm::vec3 centerOfMass;
		float eccentricity = 0.5;
		int numPoints = 1000;
		bool orbitLines = true;
        GLfloat scale = 0.1f;
        bool move = true;
        GLfloat speed = 0.1f;
        std::vector<GLuint> orbitPathVAOs;
        glm::vec3 *lightPos;

        Planet(glm::vec3 lightPositions[]) {
            lightPos = lightPositions;
            centerOfMass = (lightPositions[0] * massratio + lightPositions[1]) / totalMassRatio;
        }

        std::vector<glm::vec3> generateOrbitPathPoints(float semiMajorAxis, float semiMinorAxis) {
            std::vector<glm::vec3> points;
            for (int i = 0; i <= numPoints; ++i) {
                float angle = 2.0f * glm::pi<float>() * static_cast<float>(i) / static_cast<float>(numPoints);
                float x = semiMajorAxis * sin(angle);
                float y = semiMinorAxis * cos(angle);
                points.push_back(glm::vec3(x, 0.0f, y));
            }
            return points;
        }

        std::tuple<glm::mat4, GLfloat, GLfloat> transformPlanetModel(Shader& shader, GLuint i, float a, float r, float s,
            float rotationSpeed, glm::vec3 axis = glm::vec3(0.0f, 0.1f, 0.0f)) {
            glm::mat4 model(1);
            GLfloat angle, radius, x, y;
            angle = a * i * speed;
            radius = r * scale;

            // Calculate the position of the planet in the reference frame of the center of mass using the elliptical orbit equation
            float semiMinorAxis = radius;
            float semiMajorAxis = radius * sqrt(1.0f - eccentricity * eccentricity);
            x = semiMajorAxis * sin(glm::pi<float>() * 2 * angle / 360);
            y = semiMinorAxis * cos(glm::pi<float>() * 2 * angle / 360);
            glm::vec3 planetPosRelativeToCenterOfMass = glm::vec3(x, 0.0f, y);

            // Transform the position of the planet back to the global reference frame
            glm::vec3 planetPos = planetPosRelativeToCenterOfMass + centerOfMass;

            // Use the position of the planet in the global reference frame to update the model matrix
            model = glm::translate(model, planetPos);

            model = glm::scale(model, glm::vec3(s * scale));
            if (move) {
                angle = 0.001f * i;
                model = glm::rotate(model, angle * rotationSpeed, axis);
            }

            glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            createOrbitPathVAO(semiMajorAxis, semiMinorAxis);
            return { model, x, y };
        };

        void transformSunModel(Shader &shader, GLuint i, float a, float offset, int lightIndex, float s, float rotationSpeed) {
            float sunDistance = glm::length(*(lightPos + lightIndex) - centerOfMass);
            GLfloat angle = a * i * speed;

            // Calculate new positions for the suns
            glm::vec3 sunOffset(sunDistance * cos(angle + offset), 0.0f, sunDistance * sin(angle + offset));
            glm::vec3 sunPosition = centerOfMass + sunOffset;

            glm::mat4 model(1);
            model = glm::translate(model, *(lightPos + lightIndex));
            model = glm::scale(model, glm::vec3(s * scale));
            angle = 0.001f * i * speed;
            model = glm::rotate(model, angle * rotationSpeed, glm::vec3(0.0f, 0.1f, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            *(lightPos + lightIndex) = sunPosition;
        };

        void createOrbitPathVAO(float semiMajorAxis, float semiMinorAxis) {
            std::vector<glm::vec3> orbitPathPoints = generateOrbitPathPoints(semiMajorAxis, semiMinorAxis);

            // Create a VAO and VBO for the orbit path points
            GLuint orbitPathVAO, orbitPathVBO;
            glGenVertexArrays(1, &orbitPathVAO);
            glGenBuffers(1, &orbitPathVBO);
            glBindVertexArray(orbitPathVAO);
            glBindBuffer(GL_ARRAY_BUFFER, orbitPathVBO);
            glBufferData(GL_ARRAY_BUFFER, orbitPathPoints.size() * sizeof(glm::vec3), &orbitPathPoints[0], GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
            glBindVertexArray(0);

            orbitPathVAOs.push_back(orbitPathVAO);
        };

        void DrawOrbitLines(Shader &shader) {
            for (size_t i = 0; i < orbitPathVAOs.size(); ++i) {
                glBindVertexArray(orbitPathVAOs[i]);
                glm::mat4 orbitPathModel = glm::translate(glm::mat4(1), centerOfMass);
                glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(orbitPathModel));
                if (orbitLines) {
                    glDrawArrays(GL_LINE_STRIP, 0, numPoints + 1);
                }
            }
            glBindVertexArray(0);
        }
};
