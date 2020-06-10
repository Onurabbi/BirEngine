#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Display.h"
#include "Renderer.h"

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 300.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 70.0f;


Camera* createCamera(glm::vec3 Position, glm::vec3 WorldUp)
{
    Camera* camera = (Camera*)malloc(sizeof(Camera));
    camera->Position = Position;
    camera->WorldUp = WorldUp;
    camera->Yaw = YAW;
    camera->Pitch = PITCH;
    camera->MovementSpeed = SPEED;
    camera->MouseSensitivity = SENSITIVITY;
    camera->Zoom = ZOOM;
    updateCameraVectors(camera);
    return camera; 
}


void updateCameraVectors(Camera* camera)
{
    //calculate new front vector
    glm::vec3 front;
    front.x = cos(glm::radians(camera->Yaw))*cos(glm::radians(camera->Pitch));
    front.y = sin(glm::radians(camera->Pitch));
    front.z = sin(glm::radians(camera->Yaw))*cos(glm::radians(camera->Pitch));
    camera->Front = glm::normalize(front);
    //re-calculate right and up vector
    camera->Right = glm::normalize(glm::cross(camera->Front, camera->WorldUp));
    camera->Up = glm::normalize(glm::cross(camera->Right, camera->Front));
}

glm::mat4 createViewMatrix(Camera* camera)
{
    return glm::lookAt(camera->Position, (camera->Position)+(camera->Front), camera->Up);
}

glm::mat4 createProjectionMatrix(Camera* camera)
{
    float aspectRatio = (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT;
    float yScale = (1.0f / tan(glm::radians(camera->Zoom/2.0f)))*aspectRatio;
    float xScale = yScale/aspectRatio;
    float farPlane = FAR_PLANE;
    float nearPlane = NEAR_PLANE;
    float frustumLength = farPlane-nearPlane;

    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    projectionMatrix[0][0]=xScale;
    projectionMatrix[1][1]=yScale;
    projectionMatrix[2][2]=-((farPlane+nearPlane)/frustumLength);
    projectionMatrix[2][3]=-1;
    projectionMatrix[3][2]=-((2*nearPlane*farPlane)/frustumLength);
    projectionMatrix[3][3]=0;
    return projectionMatrix;
}

void processKeyboard(Camera* camera, Camera_Movement direction, float deltaTime)
{
    float velocity = camera->MovementSpeed*deltaTime;
    if(direction == FORWARD)
        camera->Position += (camera->Front)*velocity;
    if(direction == BACKWARD)
        camera->Position -= (camera->Front)*velocity;
    if(direction == LEFT)
        camera->Position -= (camera->Right)*velocity;
    if(direction == RIGHT)
        camera->Position += (camera->Right)*velocity;
}

void processMouseMovement(Camera* camera, float xoffset, float yoffset)
{
    xoffset *= camera->MouseSensitivity;
    yoffset *= camera->MouseSensitivity;

    camera->Yaw += xoffset;
    camera->Pitch += yoffset;

    if(camera->Pitch > 89.0f)
        camera->Pitch = 89.0f;
    if(camera->Pitch < -89.0f)
        camera->Pitch = -89.0f;

    updateCameraVectors(camera);
}

void processMouseScroll(Camera* camera, float yoffset)
{
    if(camera->Zoom >= 1.0f && camera->Zoom <= 70.0f)
        camera->Zoom -= yoffset;
    if(camera->Zoom <=1.0f)
        camera->Zoom = 1.0f;
    if(camera->Zoom > 70.0f)
        camera->Zoom = 70.0f;
}
