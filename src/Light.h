#ifndef LIGHT_H_
#define LIGHT_H

struct DirectionalLight
{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 direction;
};


struct PointLight
{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float constant;
    float linear;
    float quadratic;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;
};

DirectionalLight* createDirectionalLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse,
                                         glm::vec3 specular, glm::vec3 direction);

PointLight* createPointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, 
                             glm::vec3 specular, float constant, float linear, float quadratic);

SpotLight* createSpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 diffuse, glm::vec3 ambient,glm::vec3 specular, 
                           float constant, float linear, float quadratic, float cutOff, float outerCutOff);

void setupPointLights(PointLight**, unsigned int num_point_lights);

#endif