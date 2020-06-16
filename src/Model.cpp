
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <limits.h>
#include <string>

#include "Model.h"
#include "Level.h"
#include "Camera.h"
#include "Shader.h"

char* directorize(const char* path)
{
    char* res = (char*)malloc(1024*sizeof(char));
    int i=0; int lastIndex = 0;
    while(path[i]!='\0')
    {
        res[i]=path[i];
        if(path[i]=='/')
            lastIndex = i;
        i++;
    }
    res = (char*)realloc(res, lastIndex+2);
    res[lastIndex+1]='\0';
    return res;
}

struct Model* createModel(const char* path, bool gamma)
{
    struct Model* model = (struct Model*)malloc(sizeof(struct Model));
    loadModel(model,path);
    return model;
}

glm::mat4 createTransformationMatrix(Model* model)
{
    glm::mat4 matrix = glm::mat4(1.0f);
    glm::vec3 translation = model->position;

    float rx = model->rotx;
    float ry = model->roty;
    float rz = model->rotz;
    float scale = model->scale;

    matrix = glm::translate(matrix, translation);
    matrix = glm::rotate(matrix, glm::radians(rx), glm::vec3(1.0f, 1.0f, 1.0f));
    matrix = glm::rotate(matrix, glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(rz), glm::vec3(0.0f, 0.0f, 1.0f));

    matrix = glm::scale(matrix, glm::vec3(scale, scale, scale));
    return matrix;
}

void drawModel(Level* level, Room* room, Model* model, Shader* shader, Camera* camera)
{   
    for(unsigned int i=0; i<level->num_to_draw; ++i)
    {
        drawMesh(model, model->meshes[level->meshesToBeDrawn[i]], shader);
    }
}

void translateModel(struct Model* model, float dx, float dy, float dz)
{
    model->position.x+=dx;
    model->position.y+=dy;
    model->position.z+=dz;
}

void rotateModel(struct Model* model, float dx, float dy, float dz)
{
    model->rotx+=dx;
    model->roty+=dy;
    model->rotz+=dz;
}

void scaleModel(struct Model* model, float scale)
{
    model->scale = scale;
}

void loadModel(struct Model* model, const char* path)
{
    float lastTime = glfwGetTime();
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		printf("Error::Assimp:: %s\n", importer.GetErrorString());
	}
    
    float curTime = glfwGetTime();
    float assimpTime = curTime-lastTime;
    model->directory = directorize(path);
    //setup texture and mesh arrays
    model->num_textures = getModelTextureCount(scene);
    model->cur_num_textures = 0;
    model->num_meshes = scene->mNumMeshes;
    model->cur_num_meshes = 0;
    Texture** textures = (Texture**)malloc(model->num_textures*sizeof(Texture*));
    Mesh** meshes = (Mesh**)malloc(model->num_meshes*sizeof(Mesh*));
    model->textures = textures;
    model->meshes = meshes;
    model->position = glm::vec3(0.0f, 0.0f, 0.0f);
    model->rotx = 0.0f;
    model->roty = 0.0f;
    model->rotz = 0.0f;
    model->scale = 1;
    model->gammaCorrection = true;
    processNode(model, scene->mRootNode, scene);
}

void processNode(struct Model* model, aiNode* node, const aiScene* scene)
{
    for(unsigned int i=0; i<node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model->meshes[model->cur_num_meshes] = processMesh(model, mesh, scene);
        model->cur_num_meshes++;
    }
    for(unsigned int i=0; i<node->mNumChildren; i++)
    {
        processNode(model, node->mChildren[i], scene);
    }
}


Mesh* processMesh(Model* model, aiMesh* ai_mesh, const aiScene* scene)
{
    Mesh* mesh = (Mesh*)malloc(sizeof(Mesh));

    unsigned int num_vertices = ai_mesh->mNumVertices;
    unsigned int num_faces = (ai_mesh->mNumFaces);
    unsigned int num_indices = 0;

    for(unsigned int i=0; i<num_faces; i++)
    {
        aiFace face = ai_mesh->mFaces[i];
        unsigned int indices = face.mNumIndices;
        num_indices+=indices;
    }

    unsigned int num_textures = getMeshTextureCount(scene, ai_mesh);
    Vertex* vertices = (Vertex*)malloc(num_vertices*sizeof(Vertex));
    unsigned int* indices = (unsigned int*)malloc(num_indices*sizeof(unsigned int));
    unsigned int* textures = (unsigned int*)malloc(num_textures*sizeof(unsigned int));

    mesh->vertices = vertices;
    mesh->indices = indices;
    mesh->textures = textures;
    mesh->num_indices = num_indices;
    mesh->num_textures = num_textures;
    mesh->num_vertices = num_vertices;
    mesh->cur_num_textures = 0;
    mesh->box_id = model->cur_num_meshes;
    //placeholder values to be overriden
    mesh->VAO = 0;
    mesh->VBO = 0;
    mesh->EBO = 0;

    for(unsigned int i=0; i<num_vertices; i++)
    {
        Vertex* vertex = (Vertex*)malloc(sizeof(Vertex));
        glm::vec3 vector;

        vector.x = ai_mesh->mVertices[i].x;
        vector.y = ai_mesh->mVertices[i].y;
        vector.z = ai_mesh->mVertices[i].z;
        //Position
        vertex->Position = vector;
        //Normals
        vector.x = ai_mesh->mNormals[i].x;
        vector.y = ai_mesh->mNormals[i].y;
        vector.z = ai_mesh->mNormals[i].z;
        vertex->Normal = vector;
        //texture coordinates
        if(ai_mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = ai_mesh->mTextureCoords[0][i].x;
            vec.y = ai_mesh->mTextureCoords[0][i].y;
            vertex->TexCoords = vec;
        }
        else
            vertex->TexCoords = glm::vec2(0.0f, 0.0f);
        
        vector.x = ai_mesh->mTangents[i].x;
        vector.y = ai_mesh->mTangents[i].y;
        vector.z = ai_mesh->mTangents[i].z;
        vertex->Tangent = vector;

        vector.x = ai_mesh->mBitangents[i].x;
        vector.y = ai_mesh->mBitangents[i].y;
        vector.z = ai_mesh->mBitangents[i].z;
        vertex->BiTangent = vector;
        mesh->vertices[i] = *vertex;
        free(vertex);                                   
    }
    unsigned int k=0;

    for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++)
	{
		aiFace face = ai_mesh->mFaces[i];
		//retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++) 
        {
			mesh->indices[k] = face.mIndices[j];
            k++;
		}
	}

    aiMaterial* material = scene->mMaterials[ai_mesh->mMaterialIndex];

    loadMaterialTextures(model, mesh, material, aiTextureType_DIFFUSE, "texture_diffuse");
    loadMaterialTextures(model, mesh, material, aiTextureType_SPECULAR, "texture_specular");
    loadMaterialTextures(model, mesh, material, aiTextureType_HEIGHT, "texture_normal");
    loadMaterialTextures(model, mesh, material, aiTextureType_AMBIENT, "texture_height");
    setupMesh(mesh);
    return mesh;
}


unsigned int getMeshTextureCount(const aiScene* scene, aiMesh* mesh)
{
    unsigned int count = 0;
    aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
    count+=aiGetMaterialTextureCount(mat, aiTextureType_DIFFUSE);
    count+=aiGetMaterialTextureCount(mat, aiTextureType_SPECULAR);
    count+=aiGetMaterialTextureCount(mat, aiTextureType_HEIGHT);
    count+=aiGetMaterialTextureCount(mat, aiTextureType_AMBIENT);
    return count;
}

unsigned int getModelTextureCount(const aiScene* scene)
{
    unsigned int count = 0;
    unsigned num_materials = scene->mNumMaterials;

    for(unsigned int i=0; i<num_materials; i++)
    {
        aiMaterial* mat = scene->mMaterials[i];
        count+=aiGetMaterialTextureCount(mat, aiTextureType_DIFFUSE);
        count+=aiGetMaterialTextureCount(mat, aiTextureType_SPECULAR);
        count+=aiGetMaterialTextureCount(mat, aiTextureType_HEIGHT);
        count+=aiGetMaterialTextureCount(mat, aiTextureType_AMBIENT);
    }
    return count;
}

void loadMaterialTextures(Model* model, Mesh* mesh, aiMaterial* mat, aiTextureType type, char* typeName)
{
    for(unsigned int i=0; i<mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool gamma;
        if(strcmp(typeName, "texture_diffuse")==0) gamma = true;
        else gamma = false;

        bool skip = false;
        const char* assimpPath = str.C_Str();
        size_t len = strlen(assimpPath);

        for(unsigned int j=0; j<model->cur_num_textures; j++)
        {
            const char* modelPath = model->textures[j]->path;
            if(memcmp(modelPath, assimpPath, len)==0)
            {
                mesh->textures[mesh->cur_num_textures++] = j;
                skip = true;
                break;
            }
        }
        if(!skip)
        {
            Texture* texture = (Texture*)malloc(sizeof(Texture));
            
            texture->id = TextureFromFile(str.C_Str(), model->directory, gamma);
            texture->type = stringCopy(typeName);
            texture->path = stringCopy(str.C_Str());
            model->textures[model->cur_num_textures++] = texture;
            mesh->textures[mesh->cur_num_textures++] = (model->cur_num_textures-1);
        }
    }
}

unsigned int TextureFromFile(const char* path, const char* directory, bool gamma)
{
    char* filename = (char*)malloc(1024*sizeof(char));
    unsigned int len = strlen(path);
    len+=strlen(directory);
    strcpy(filename, directory);
    strcat(filename, path);
    filename = (char*)realloc(filename, len+1);
    
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename, &width, &height, &nrComponents, 0);

    if(data)
    {
        GLenum internalFormat;
        GLenum dataFormat;

        if(nrComponents == 1)
            internalFormat = dataFormat = GL_RED;
        if(nrComponents == 3)
        {
            internalFormat = gamma ? GL_SRGB : GL_RGB;
            dataFormat = GL_RGB;
        }
        if(nrComponents == 4)
        {
            internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
            dataFormat = GL_RGBA;
        }
            
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
        printf("Texture failed to load at path: %s\n", path);
    free(filename);
    return textureID;
}
//TODO: complete this
unsigned int loadCubeMap(char** faces, unsigned int num_faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for(unsigned int i=0; i<num_faces; i++)
    {
        unsigned char* data = stbi_load(faces[i], &width, &height, &nrChannels, 0);
        if(data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            printf("Cubemap texture failed to load at path: %s\n", faces[i]);
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

BoundingBox* createBoundingBox(glm::vec3 v1, glm::vec3 v7)
{
    BoundingBox* box = (BoundingBox*)malloc(sizeof(BoundingBox));
    box->v[0] = v1;
    box->v[1] = glm::vec3(v7.x, v1.y, v1.z);
    box->v[2] = glm::vec3(v7.x, v7.y, v1.z);
    box->v[3] = glm::vec3(v1.x, v7.y, v1.z);
    box->v[4] = glm::vec3(v1.x, v1.y, v7.z);
    box->v[5] = glm::vec3(v7.x, v1.y, v7.z);
    box->v[6] = v7;
    box->v[7] = glm::vec3(v1.x, v7.y, v7.z);
    return box;
}

Portal* createPortal(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
    Portal* portal = (Portal*)malloc(sizeof(Portal));
    portal->v[0] = v0;
    portal->v[1] = v1;
    portal->v[2] = v2;
    portal->v[3] = v3;
    return portal;
}

void updateBoundingBox(BoundingBox* box, glm::vec3 vector)
{
    if(vector.x < box->v[0].x)       
        {box->v[0].x = vector.x; box->v[3].x = vector.x; box->v[4].x = vector.x; box->v[7].x = vector.x;}
    if(vector.y < box->v[0].y)
        {box->v[0].y = vector.y; box->v[1].y = vector.y; box->v[4].y = vector.y; box->v[5].y = vector.y;}
    if(vector.z < box->v[0].z)
        {box->v[0].z = vector.z; box->v[1].z = vector.z; box->v[2].z = vector.z; box->v[3].z = vector.z;}
    if(vector.x > box->v[6].x)
        {box->v[6].x = vector.x; box->v[1].x = vector.x; box->v[2].x = vector.x; box->v[5].x = vector.x;}
    if(vector.y > box->v[6].y)
        {box->v[6].y = vector.y; box->v[2].y = vector.y; box->v[3].y = vector.y; box->v[7].y = vector.y;}
    if(vector.z > box->v[6].z)
        {box->v[6].z = vector.z; box->v[4].z = vector.z; box->v[5].z = vector.z; box->v[7].z = vector.z;}
}

void setupMesh(Mesh* mesh)
{
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    mesh->VAO = VAO;
    mesh->EBO = EBO;
    mesh->VBO = VBO;

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->num_vertices*sizeof(Vertex), &(mesh->vertices[0]), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->num_indices*sizeof(unsigned int), &(mesh->indices[0]), GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BiTangent));

    glBindVertexArray(0);
}
//TODO: complete skybox rendering
void setupSkyBox(Mesh* skybox)
{
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);

    skybox->VBO = skyboxVBO;
    skybox->VAO = skyboxVAO;

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, skybox->num_vertices*sizeof(float), &(skybox->vertices[0]), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void drawMesh(Model* model, Mesh* mesh, Shader* shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    unsigned int nr_textures = mesh->num_textures;
    
    for(unsigned int i=0; i<nr_textures; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        char buffer[4];
        char* temp = (char*)malloc(1024*sizeof(char));

        const char* name = stringCopy(model->textures[mesh->textures[i]]->type);

        if(strcmp(name, "texture_diffuse") == 0)
            sprintf(buffer, "%d", diffuseNr++);
        if(strcmp(name, "texture_specular")==0)
            sprintf(buffer, "%d", specularNr++);
        if(strcmp(name, "texture_normal")==0)
            sprintf(buffer, "%d", normalNr++);
        if(strcmp(name, "texture_height")==0)
            sprintf(buffer, "%d", heightNr++);
        strcpy(temp, name);
        strcat(temp, buffer);
        unsigned int len = strlen(temp);
        temp = (char*)realloc(temp, len+1);
        unsigned int loc = glGetUniformLocation(shader->ID, temp);
        glUniform1i(loc, i);
        glBindTexture(GL_TEXTURE_2D, model->textures[mesh->textures[i]]->id);

        free(temp);
    }

    glBindVertexArray(mesh->VAO);

    glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glActiveTexture(0);
}

const char* stringCopy(const char* str)
{
    char* res = (char*)malloc(1024*sizeof(char));
    unsigned int len = strlen(str);
    strcpy(res, str);
    res[len] = '\0';
    res = (char*)realloc(res, len+1);
    return res;
}