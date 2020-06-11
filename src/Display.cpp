#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Display.h"

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

extern Camera* camera;

GLFWwindow* createDisplay()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BirEngineC", NULL, NULL);

    if(window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return NULL;
    }
    return window;
}

void closeDisplay()
{
    glfwTerminate();
}

void updateDisplay(GLFWwindow* window)
{
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void setCallbackFunctions(GLFWwindow* window, struct Camera* camera)
{
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    processMouseMovement(camera, xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    processMouseScroll(camera, yoffset);
}

void processInput(GLFWwindow* window, struct Camera* camera, float deltaTime)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_W)==GLFW_PRESS)
        processKeyboard(camera, FORWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S)==GLFW_PRESS)
        processKeyboard(camera, BACKWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A)==GLFW_PRESS)
        processKeyboard(camera, LEFT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D)==GLFW_PRESS)
        processKeyboard(camera, RIGHT, deltaTime);
}
