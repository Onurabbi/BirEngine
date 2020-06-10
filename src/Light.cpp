
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Light.h"

DirectionalLight* createDirectionalLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse,
                                         glm::vec3 specular, glm::vec3 direction)
{
    DirectionalLight* dirLight = (DirectionalLight*)malloc(sizeof(DirectionalLight));
    dirLight->position = position;
    dirLight->ambient = ambient;
    dirLight->diffuse = diffuse;
    dirLight->specular = specular;
    dirLight->direction = direction;
    return dirLight;
}

PointLight* createPointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, 
                             glm::vec3 specular, float constant, float linear, float quadratic)
{
    PointLight* pointLight = (PointLight*)malloc(sizeof(PointLight));
    pointLight->position = position;
    pointLight->ambient = ambient;
    pointLight->diffuse = diffuse;
    pointLight->specular = specular;
    pointLight->linear = linear;
    pointLight->constant = constant;
    pointLight->quadratic = quadratic;
    return pointLight;
}

SpotLight* createSpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 ambient, glm::vec3 specular, 
                           float constant, float linear, float quadratic, float cutOff, float outerCutOff)
{
    SpotLight* spotLight = (SpotLight*)malloc(sizeof(SpotLight));
    spotLight->position = position;
    spotLight->ambient = ambient;
    spotLight->diffuse = diffuse;
    spotLight->specular = specular;
    spotLight->constant = constant;
    spotLight->linear = linear;
    spotLight->quadratic = quadratic;
    spotLight->direction = direction;
    spotLight->cutOff = cutOff;
    spotLight->outerCutOff = outerCutOff;
    return spotLight;
}

void setupPointLights(PointLight** pointLightArr, unsigned int num_point_lights)
{
    pointLightArr[0]=createPointLight(glm::vec3(-560.0f, 240.0f, -120.0f),
                                      glm::vec3(0.1f, 0.1f, 0.1f),
                                      glm::vec3(0.8f, 0.8f, 0.8f),
                                      glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.0014f, 0.000007f);
    pointLightArr[1]=createPointLight(glm::vec3(810.0f, 190.0f, 240.0f),
                                      glm::vec3(0.1f, 0.1f, 0.1f),
                                      glm::vec3(0.8f, 0.8f, 0.8f),
                                      glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.0014f, 0.000007f);
}