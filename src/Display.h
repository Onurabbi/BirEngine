#ifndef DISPLAY_H_
#define DISPLAY_H_

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

GLFWwindow* createDisplay();

void closeDisplay();

void updateDisplay(GLFWwindow* window);

void setCallbackFunctions(GLFWwindow* window, struct Camera* camera);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow* window, struct Camera* camera, float deltaTime);

#endif