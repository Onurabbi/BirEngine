#ifndef MODEL_H_
#define MODEL_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Level;
struct Room;
struct Camera;

char* directorize(const char* path);

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 BiTangent;
};

struct BoundingBox {
    glm::vec3 v[8];
};

struct Texture {
    unsigned int id;
    const char* type;
    const char* path;
};

struct Portal {
    glm::vec3 v[4];
};

struct Frustum {
    float left, right, bottom, top;
};

struct Mesh
{
    Vertex* vertices;
    unsigned int* indices;
    unsigned int* textures; //indices of textures

    unsigned int num_vertices;
    unsigned int num_indices;
    unsigned int num_textures;

    unsigned int cur_num_textures;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    unsigned int box_id;
    bool visibility;
};

struct Model{

    Texture** textures;
    Mesh** meshes;

    unsigned int num_textures;
    unsigned int cur_num_textures;
    unsigned int num_meshes;
    unsigned int cur_num_meshes;

    const char* directory;
    BoundingBox* box;

    bool gammaCorrection;
    glm::vec3 position;

    float rotx, roty, rotz;
    float scale;
};

const char* stringCopy(const char* str);

void setupMesh(Mesh* mesh);

void drawMesh(struct Model* model, struct Mesh* mesh, struct Shader* shader);

Model* createModel(const char* path, bool gamma);

glm::mat4 createTransformationMatrix(Model* model);

void drawModel(Level* level, Room* room, Model* model, Shader* shader, Camera* camera);

void translateModel(struct Model* model, float dx, float dy, float dz);

void rotateModel(struct Model* model, float dx, float dy, float dz);

void scaleModel(struct Model* model, float scale);

void loadModel(struct Model* model, const char* path);

void processNode(struct Model* model, aiNode* node, const aiScene* scene);

Mesh* processMesh(Model* model, aiMesh* ai_mesh, const aiScene* scene);

unsigned int getMeshTextureCount(const aiScene* scene, aiMesh* mesh);

unsigned int getModelTextureCount(const aiScene* scene);

void loadMaterialTextures(Model* model, Mesh* mesh, aiMaterial* mat, aiTextureType type, char* typeName);

unsigned int TextureFromFile(const char* path, const char* directory, bool gamma);

BoundingBox* createBoundingBox(glm::vec3 v1, glm::vec3 v7);

Portal* createPortal(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4);

void updateBoundingBox(BoundingBox* box, glm::vec3 vector);

#endif