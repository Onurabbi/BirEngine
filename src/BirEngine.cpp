#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Display.h"
#include "Camera.h"
#include "Level.h"
#include "Model.h"
#include "Renderer.h"
#include "Light.h"
#include "Shader.h"

void writeFile(Model* model, Level* level);

Camera* camera = createCamera(glm::vec3(-1340.0f,140.0f, -100.0f), glm::vec3(0.0f, 1.0f, 0.0f));
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    GLFWwindow* window = createDisplay();
    setCallbackFunctions(window, camera);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    initiateRenderer();
    Shader* shader = createShader("Shaders/vertexShader.vs", "Shaders/fragmentShader.fs");
    Model* levelModel = createModel("assets/Medieval/Demonstration.obj", false);
    Level* level = createLevel("level.txt", levelModel);
    //lighting
    DirectionalLight* dirLight = createDirectionalLight(glm::vec3(200000.0f, 40000.0f, 20000.0f),
                                                        glm::vec3(0.1f, 0.1f, 0.1f),
                                                        glm::vec3(0.8f, 0.8f, 0.8f),
                                                        glm::vec3(0.5, 0.5f, 0.5f),
                                                        glm::vec3(1, 1, 1));

    int num_point_lights = 2;
    PointLight* pointLightArr[2];
    setupPointLights(pointLightArr, num_point_lights);
    SpotLight* spotLight = createSpotLight(camera->Position, camera->Front, glm::vec3(0.402f, 0.65f, 1.0f),
                                           glm::vec3(0.0f, 0.0f, 0.0f),
                                           glm::vec3(0.402f, 0.65f, 1.0f),
                                           1.0f, 0.0014f, 0.000007f, 15.0f, 25.0f);
    //TODO: complete skybox implementation
    /*char* faces[6] = 
    {   "assets/Skybox/right.jpg",
        "assets/Skybox/left.jpg",
        "assets/Skybox/top.jpg",
        "assets/Skybox/bottom.jpg",
        "assets/Skybox/front.jpg",
        "assets/Skybox/back.jpg"
    };*/
    
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame-lastFrame;
        lastFrame = currentFrame;
        float fps = 1.0f/deltaTime;
        printf("FPS: %f\n", fps);
        processInput(window, camera, deltaTime);
        prepareRenderer();
        use(shader);
        loadDirectionalLight(shader, dirLight);
        loadPointLight(shader, pointLightArr, num_point_lights);
        loadSpotLight(shader, spotLight, camera);
        render(level, levelModel, shader, camera);
        stop(shader);
        updateDisplay(window);
        resetLevel(level);
    }

    closeDisplay();
    return 0;
}

