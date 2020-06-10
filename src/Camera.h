#ifndef CAMERA_H_
#define CAMERA_H_


extern const float YAW;
extern const float PITCH;
extern const float SPEED;
extern const float SENSITIVITY;
extern const float ZOOM;

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

struct Camera
{
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
};

Camera* createCamera(glm::vec3 Position, glm::vec3 WorldUp);

void updateCameraVectors(Camera* camera);

glm::mat4 createViewMatrix(Camera* camera);

glm::mat4 createProjectionMatrix(Camera* camera);

void processKeyboard(Camera* camera, Camera_Movement direction, float deltaTime);

void processMouseMovement(Camera* camera, float xoffset, float yoffset);

void processMouseScroll(Camera* camera, float yoffset);

#endif