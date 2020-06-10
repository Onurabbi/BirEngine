#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <stdio.h>

#include "Camera.h"
#include "Model.h"
#include "Level.h"
#include "Shader.h"
#include "Renderer.h"

void initiateRenderer()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void prepareRenderer()
{
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1);
}

void render(Level* level, Model* model, Shader* shader, Camera* camera)
{
    glm::mat4 modelMatrix = createTransformationMatrix(model);
    glm::mat4 projectionMatrix = createProjectionMatrix(camera);
    glm::mat4 viewMatrix = createViewMatrix(camera);
    glm::mat4 comboMatrix = projectionMatrix*viewMatrix;
    //send data to shader
    setMat4(shader, "model", modelMatrix);
    setMat4(shader, "projection", projectionMatrix);
    setMat4(shader, "view", viewMatrix);
    setVec3(shader, "viewPos", camera->Position);
    setFloat(shader, "shininess", 64.0);


    for(unsigned int i=0; i<level->num_rooms; i++)
    {
        if(pointInsideBox(level->roomBoxes[i], camera->Position))
        {
            Frustum frustum = {-1.0f, 1.0f, -1.0f, 1.0f};
            processRoom(level, level->rooms[i], &frustum, camera, comboMatrix);
            drawModel(level, level->rooms[i], model, shader, camera);
            break;
        }  
    }
}
