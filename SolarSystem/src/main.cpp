#include <iostream>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Camera.h"
#include "Shader.h"
#include "Planet.h"
#include "Model.h"
#include <stdio.h>
#include "Texture.h"
#include "Skybox.h"
using namespace std;

int SCREEN_WIDTH = 1000;
int SCREEN_HEIGHT = 750;

// Light attributes
glm::vec3 lightPositions[] =
{
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, -20.0f)
};

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement(Planet& planetHelper);

// Camera
Camera camera(glm::vec3(-20.0f, 10.0f, 10.0f));
bool keys[1024];
GLfloat lastX = (float)SCREEN_WIDTH / 2.0, lastY = (float)SCREEN_HEIGHT / 2.0;
bool firstMouse = true;
string cameraType = "";

// Time
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Solar System", nullptr, nullptr);

    if (nullptr == window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();

        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    //Load Shaders
    Shader modelShader("../resources/shaders/modelLoading.vs", "../resources/shaders/modelLoading.frag");
    Shader lampShader("../resources/shaders/lamp.vs", "../resources/shaders/lamp.frag");
    Shader lineShader("../resources/shaders/line.vs", "../resources/shaders/line.frag");
    Shader skyboxShader("../resources/shaders/skybox.vs", "../resources/shaders/skybox.frag");

    // Load models    
    Model earthModel("../resources/models/earth/Earth.obj");
    Model sunModel1("../resources/models/sun/sun.obj");
    Model sunModel2("../resources/models/sun2/sun.obj");
    Model mercuryModel("../resources/models/mercury/mercury.obj");
    Model venusModel("../resources/models/venus/venus.obj");
    Model marsModel("../resources/models/mars/mars.obj");
    Model jupiterModel("../resources/models/jupiter/jupiter.obj");
    Model saturnModel("../resources/models/saturn/13906_Saturn_v1_l3.obj");
    Model uranusModel("../resources/models/uranus/13907_Uranus_v2_l3.obj");
    Model neptuneModel("../resources/models/neptune/13908_Neptune_V2_l3.obj");

    // Planets    
    Planet planetHelper(lightPositions);
    glm::vec3 centerOfMass = planetHelper.centerOfMass;

    //Skybox
    Skybox skybox;
    unsigned int cubemapTexture = TextureLoading::LoadCubemap(skybox.faces);

    // Perspective Projection
    glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    GLuint i = 0;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (planetHelper.move) {
            i++;
        }       
        glfwPollEvents();
        DoMovement(planetHelper);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view(1);
        view = camera.GetViewMatrix();
        std::tuple<glm::mat4, GLfloat, GLfloat> modelAndCoordinates;        
        glm::mat4 model(1);        

        //PLANETS

        GLfloat angle, radius, x, y;
        modelShader.Use();
        GLint viewPosLoc = glGetUniformLocation(modelShader.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);        

        // Set lights properties

        //PointLight 1
        glUniform3f(glGetUniformLocation(modelShader.Program, "lights[0].position"), lightPositions[0].x, lightPositions[0].y, lightPositions[0].z);
        glUniform3f(glGetUniformLocation(modelShader.Program, "lights[0].ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(modelShader.Program, "lights[0].diffuse"), 4.5f, 4.5f, 4.5f);
        glUniform3f(glGetUniformLocation(modelShader.Program, "lights[0].specular"), 0.0f, 0.0f, 0.0f);
        glUniform1f(glGetUniformLocation(modelShader.Program, "lights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(modelShader.Program, "lights[0].linear"), 0.02f);
        glUniform1f(glGetUniformLocation(modelShader.Program, "lights[0].quadratic"), 0.006f);

        //PointLight 2
        glUniform3f(glGetUniformLocation(modelShader.Program, "lights[1].position"), lightPositions[1].x, lightPositions[1].y, lightPositions[1].z);
        glUniform3f(glGetUniformLocation(modelShader.Program, "lights[1].ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(modelShader.Program, "lights[1].diffuse"), 2.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(modelShader.Program, "lights[1].specular"), 0.0f, 0.0f, 0.0f);
        glUniform1f(glGetUniformLocation(modelShader.Program, "lights[1].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(modelShader.Program, "lights[1].linear"), 0.02f);
        glUniform1f(glGetUniformLocation(modelShader.Program, "lights[1].quadratic"), 0.006f);

        glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Mercury
        modelAndCoordinates = planetHelper.transformPlanetModel(modelShader, i, 1.0f, 250.0f, 0.3f, 40.0f);
        mercuryModel.Draw(modelShader);

        // Venus
        modelAndCoordinates = planetHelper.transformPlanetModel(modelShader, i, 0.9f, 270.0f, 0.5f, 40.0f);
        venusModel.Draw(modelShader);

        // Earth
        modelAndCoordinates = planetHelper.transformPlanetModel(modelShader, i, 0.8f, 290.0f, 0.5f, 40.0f);
        earthModel.Draw(modelShader);

        if (cameraType == "Earth") {
            glm::vec3 cameraPosition = (glm::vec3(get<1>(modelAndCoordinates), 0.2f, get<2>(modelAndCoordinates)) + centerOfMass);
            camera.SetPosition(cameraPosition);

            // Calculate the direction vector pointing towards the center of mass
            glm::vec3 direction = glm::normalize(centerOfMass - cameraPosition);
            float newYaw = glm::degrees(atan2(direction.z, direction.x));
            float newPitch = glm::degrees(asin(direction.y));
            camera.SetOrientation(newYaw, newPitch);
        }

        // Mars
        modelAndCoordinates = planetHelper.transformPlanetModel(modelShader, i, 0.7f, 310.0f, 0.3f, 40.0f);
        marsModel.Draw(modelShader);

        // Jupiter
        modelAndCoordinates = planetHelper.transformPlanetModel(modelShader, i, 0.6f, 360.0f, 4.0f, 30.0f);
        jupiterModel.Draw(modelShader);

        // Saturn  
        modelAndCoordinates = planetHelper.transformPlanetModel(modelShader, i, 0.5f, 430.0f, 0.032f, 20.0f, glm::vec3(0.0f, 0.5f, -0.35f));
        model = get<0>(modelAndCoordinates);
        model = glm::rotate(model, 61.0f, glm::vec3(0.0f, 0.1f, 0.0f));
        model = glm::rotate(model, 90.0f, glm::vec3(0.1f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        saturnModel.Draw(modelShader);

        // Uranus
        modelAndCoordinates = planetHelper.transformPlanetModel(modelShader, i, 0.4f, 480.0f, 0.03f, 10.0f);
        uranusModel.Draw(modelShader);

        // Neptune
        modelAndCoordinates = planetHelper.transformPlanetModel(modelShader, i, 0.3f, 530.0f, 0.03f, 10.0f);
        neptuneModel.Draw(modelShader);

        //Orbit Lines
        lineShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(lineShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lineShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        planetHelper.DrawOrbitLines(lineShader);

        // SUNS
        lampShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        //Sun 1        
        planetHelper.transformSunModel(lampShader, i, 0.01f, 0.0f, 0, 20.0f, 20.0f);
        sunModel1.Draw(lampShader);

        //Sun 2        
        planetHelper.transformSunModel(lampShader, i, 0.01f, glm::pi<float>(), 1, 7.0f, 30.0f);
        sunModel2.Draw(lampShader);

        //over the sun
        if (cameraType == "Up") {
            camera.SetPosition(glm::vec3(-3.0f, 85.0f, -5.0f));
            camera.SetOrientation(0.0f, -88.0f);
        }    

        // Draw skybox
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));  // remove translation component
        skybox.DrawSkybox(skyboxShader, cubemapTexture, view, projection);
        
        glfwSwapBuffers(window);
        
    }
    glfwTerminate();
    return 0;
};

void DoMovement(Planet &planetHelper) {

    if (keys[GLFW_KEY_W]) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (keys[GLFW_KEY_UP]) {
        camera.ProcessKeyboard(UP, deltaTime);
    }

    if (keys[GLFW_KEY_S]) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (keys[GLFW_KEY_DOWN]) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }

    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (keys[GLFW_KEY_MINUS]) {
        camera.DecreaseSpeed();
    }

    if (keys[GLFW_KEY_EQUAL]) {
        camera.IncreaseSpeed();
    }
    else if (keys[GLFW_KEY_1]) {
        cameraType = "Up";
    }
    else if (keys[GLFW_KEY_2]) {
        cameraType = "";
    }
    else if (keys[GLFW_KEY_3]) {
        cameraType = "Earth";
    }
    else if (keys[GLFW_KEY_M]) {
        planetHelper.speed += 0.01f;
        std::cout << "SPEED : " << planetHelper.speed << std::endl;
    }
    else if (keys[GLFW_KEY_N]) {
        planetHelper.speed -= 0.01f;
        if (planetHelper.speed <= 0.0001f) {
            planetHelper.speed = 0.001f;
        }
        std::cout << "SPEED : " << planetHelper.speed << std::endl;
    }
    else if (keys[GLFW_KEY_P]) {
        planetHelper.move = !planetHelper.move;
        std::cout << "Pause/Unpaused movement" << std::endl;
    }
    else if (keys[GLFW_KEY_O]) {
        planetHelper.orbitLines = !planetHelper.orbitLines;
        std::cout << "Show/UnShow OrbitLines" << std::endl;
    }
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos) {
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}