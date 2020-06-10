#ifndef SHADER_H_
#define SHADER_H_

struct DirectionalLight;
struct PointLight;
struct SpotLight;
struct Camera;

struct Shader
{
    unsigned int ID, vertex, fragment;
};

char* readFile(const char* filePath);

Shader* createShader(char* vertexPath, char* fragmentPath);

void use(Shader* shader);
void stop(Shader* shader);

//TODO:Load lights
//--------------------------------------------------------------

void setBool(struct Shader* shader, const char* name, bool value);

void setInt(struct Shader* shader, const char* name, int value);

void setFloat(struct Shader* shader, const char* name, float value);

void setVec2(struct Shader* shader, const char* name, const glm::vec2 value);

void setVec2(struct Shader* shader, const char* name, float x, float y);


void setVec3(struct Shader* shader, const char* name, const glm::vec3 value);

void setVec3(struct Shader* shader, const char* name, float x, float y, float z);

void setVec4(struct Shader* shader, const char* name, const glm::vec4 value);

void setVec4(struct Shader* shader, const char* name, float x, float y, float z, float w);

void setMat2(struct Shader* shader, const char* name, const glm::mat2 mat);

void setMat3(struct Shader* shader, const char* name, const glm::mat3 mat);

void setMat4(struct Shader* shader, const char* name, const glm::mat4 mat);

void cleanUp(struct Shader* shader);

void bindAttribute(struct Shader* shader, unsigned int attribute, const char* variableName);

void loadDirectionalLight(Shader* shader, DirectionalLight* dirLight);

void loadPointLight(Shader* shader, PointLight** pointLightArr, unsigned int num_lights);

void loadSpotLight(Shader* shader, SpotLight* spotLight, Camera* camera);

char* makeParameter(const char* lightType, unsigned int i, const char* param);
#endif