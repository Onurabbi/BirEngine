#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>

#include "Shader.h"
#include "Camera.h"
#include "Light.h"

char* readFile(const char* filePath)
{
    //TODO: error handling
    FILE* file = fopen(filePath, "r");
    char c;
    char* str = (char*)malloc(10000*sizeof(char));
    int i = 0;
    while((c = getc(file))!= EOF)
    {
        str[i] = c;
        i++;
    }
    str[i]='\0';
    str = (char*)realloc(str, i+1);
    fclose(file);
    return str;
}

Shader* createShader(char* vertexPath, char* fragmentPath)
{
    Shader* shader = (Shader*)malloc(sizeof(Shader));

    char* vertexCode = readFile(vertexPath);
    char* fragmentCode = readFile(fragmentPath);
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    //vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    shader->vertex = vertex;
    glShaderSource(vertex, 1, &vertexCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    printf("vertex: %d", success);

    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        printf("Error::shader::vertex::compilation::failed\n%s\n", infoLog);
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    shader->fragment = fragment;
    glShaderSource(fragment, 1, &fragmentCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    printf("fragment: %d", success);
    if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		printf("Error::shader::vertex::compilation::failed\n%s\n", infoLog);
	}
    shader->ID = glCreateProgram();
    glAttachShader(shader->ID, vertex);
    glAttachShader(shader->ID, fragment);
    glLinkProgram(shader->ID);
    //print linking errors
    glGetProgramiv(shader->ID, GL_LINK_STATUS, &success);

    if(!success)
    {
        glGetProgramInfoLog(shader->ID, 512, NULL, infoLog);
        printf("Error::shader::program::linkin_failed\n%s\n", infoLog);
    }

    free(vertexCode);
    free(fragmentCode);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return shader;
}

void use(struct Shader* shader)
{
    glUseProgram(shader->ID);
}

void stop(Shader* shader)
{
    glUseProgram(0);
}
//TODO:Load lights
//--------------------------------------------------------------

void setBool(struct Shader* shader, const char* name, bool value) 
{
    glUniform1i(glGetUniformLocation(shader->ID, name), (int)value);
}

void setInt(struct Shader* shader, const char* name, int value) 
{
    glUniform1i(glGetUniformLocation(shader->ID, name), value);
}

void setFloat(struct Shader* shader, const char* name, float value)
{
    glUniform1f(glGetUniformLocation(shader->ID, name), value);
}

void setVec2(struct Shader* shader, const char* name, const glm::vec2 value)
{
    glUniform2fv(glGetUniformLocation(shader->ID, name), 1, &value[0]);
}
void setVec2(struct Shader* shader, const char* name, float x, float y)
{
    glUniform2f(glGetUniformLocation(shader->ID, name), x, y);
}

void setVec3(struct Shader* shader, const char* name, const glm::vec3 value)
{
    glUniform3fv(glGetUniformLocation(shader->ID, name),1, &value[0]);
}

void setVec3(struct Shader* shader, const char* name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(shader->ID, name), x,y,z);
}
void setVec4(struct Shader* shader, const char* name, const glm::vec4 value)
{
    glUniform4fv(glGetUniformLocation(shader->ID, name), 1, &value[0]);
}
void setVec4(struct Shader* shader, const char* name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(shader->ID, name), x,y,z,w);
}
void setMat2(struct Shader* shader, const char* name, const glm::mat2 mat)
{
    glUniformMatrix2fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE, &mat[0][0]);
}
void setMat3(struct Shader* shader, const char* name, const glm::mat3 mat)
{
    glUniformMatrix3fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE, &mat[0][0]);
}
void setMat4(struct Shader* shader, const char* name, const glm::mat4 mat)
{
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE, &mat[0][0]);
}
void cleanUp(struct Shader* shader)
{
    glUseProgram(0);
	glDetachShader(shader->ID, shader->vertex);
	glDetachShader(shader->ID, shader->fragment);
	glDeleteShader(shader->vertex);
	glDeleteShader(shader->fragment);
	glDeleteProgram(shader->ID);
}
void bindAttribute(struct Shader* shader, unsigned int attribute, const char* variableName)
{
	glBindAttribLocation(shader->ID, attribute, variableName);
}

void loadDirectionalLight(Shader* shader, DirectionalLight* dirLight)
{
    setVec3(shader, "dirLight.direction", dirLight->direction);
    setVec3(shader, "dirLight.position", dirLight->position);
    setVec3(shader, "dirLight.ambient", dirLight->ambient);
    setVec3(shader, "dirLight.diffuse", dirLight->diffuse);
    setVec3(shader, "dirLight.specular", dirLight->specular);
}

void loadPointLight(Shader* shader, PointLight** pointLightArr, unsigned int num_lights)
{

    for(unsigned int i=0; i<num_lights; i++)
    {
        char* position = makeParameter("pointLights[", i, "].position");
        char* ambient = makeParameter("pointLights[", i, "].ambient");
        char* diffuse = makeParameter("pointLights[", i, "].diffuse");
        char* specular = makeParameter("pointLights[", i, "].specular");
        char* constant = makeParameter("pointLights[", i, "].constant");
        char* linear = makeParameter("pointLights[", i, "].linear");
        char* quadratic = makeParameter("pointLights[", i, "].quadratic");
        setVec3(shader, position, pointLightArr[i]->position);
        setVec3(shader, ambient, pointLightArr[i]->ambient);
        setVec3(shader, diffuse, pointLightArr[i]->diffuse);
        setVec3(shader, specular, pointLightArr[i]->specular);
        setFloat(shader, constant, pointLightArr[i]->constant);
        setFloat(shader, linear, pointLightArr[i]->linear);
        setFloat(shader, quadratic, pointLightArr[i]->quadratic);
        free(position);
        free(ambient);
        free(diffuse);
        free(specular);
        free(constant);
        free(linear);
        free(quadratic);
    }
}

void loadSpotLight(Shader* shader, SpotLight* spotLight, Camera* camera)
{
    	setVec3(shader, "spotLight.position", camera->Position);
		setVec3(shader, "spotLight.direction", camera->Front);
		setVec3(shader, "spotLight.ambient", spotLight->ambient);
		setVec3(shader, "spotLight.diffuse", spotLight->diffuse);
		setVec3(shader, "spotLight.specular", spotLight->specular);
		setFloat(shader, "spotLight.constant", spotLight->constant);
		setFloat(shader, "spotLight.linear", spotLight->linear);
		setFloat(shader, "spotLight.quadratic", spotLight->quadratic);
		setFloat(shader, "spotLight.cutOff", spotLight->cutOff);
		setFloat(shader, "spotLight.outerCutOff", spotLight->outerCutOff);
}

char* makeParameter(const char* lightType, unsigned int i, const char* param)
{
    char* buffer = (char*)malloc(128*sizeof(char));

    char index[5];
    sprintf(index, "%d", i);

    strcpy(buffer, lightType);
    strcat(buffer, index);
    strcat(buffer, param);
    size_t len = strlen(buffer);
    buffer = (char*)realloc(buffer, len+1);
    return buffer;
}


