#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Display.h"
#include "Camera.h"
#include "Level.h"
#include "Model.h"
#include "Renderer.h"
#include "Light.h"
#include "Shader.h"

void writeFile(Model* model, Level* level);

Camera* camera = createCamera(glm::vec3(-1340.0f,140.0f, -100.0f), glm::vec3(0.0f, 1.0f, 0.0f));
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    GLFWwindow* window = createDisplay();
    setCallbackFunctions(window, camera);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    initiateRenderer();
    Shader* shader = createShader("Shaders/vertexShader.vs", "Shaders/fragmentShader.fs");
    Model* levelModel = createModel("assets/Medieval/Demonstration.obj", false);
    Level* level = createLevel("level.txt", levelModel);
    //lighting
    DirectionalLight* dirLight = createDirectionalLight(glm::vec3(200000.0f, 40000.0f, 20000.0f),
                                                        glm::vec3(1.0f,1.0f,1.0f),
                                                        glm::vec3(0.1f, 0.1f, 0.1f),
                                                        glm::vec3(0.8f, 0.8f, 0.8f),
                                                        glm::vec3(1.0f, 1.0f, 1.0f));

    int num_point_lights = 2;
    PointLight* pointLightArr[2];
    setupPointLights(pointLightArr, num_point_lights);
    SpotLight* spotLight = createSpotLight(camera->Position, camera->Front, glm::vec3(0.402f, 0.65f, 1.0f),
                                           glm::vec3(0.0f, 0.0f, 0.0f),
                                           glm::vec3(0.402f, 0.65f, 1.0f),
                                           1.0f, 0.0014f, 0.000007f, 15.0f, 25.0f);

    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame-lastFrame;
        lastFrame = currentFrame;
        float fps = 1.0f/deltaTime;
        printf("FPS: %f\n", fps);
        processInput(window, camera, deltaTime);
        prepareRenderer();
        use(shader);
        loadDirectionalLight(shader, dirLight);
        loadPointLight(shader, pointLightArr, num_point_lights);
        loadSpotLight(shader, spotLight, camera);
        render(level, levelModel, shader, camera);
        stop(shader);
        updateDisplay(window);
        resetLevel(level);
    }

    closeDisplay();
    return 0;
}

//TODO:delete this
void writeFile(Model* model, Level* level)
{
    FILE* fp = fopen("ErrorLog.txt", "w");
    fprintf(fp, "------------------------------------------------------------\n");
    fprintf(fp, "Model info:\n");
    fprintf(fp, "Number of Textures: %d\n", model->num_textures);
    fprintf(fp, "Current number of textures: %d\n", model->cur_num_textures);
    fprintf(fp, "Number of meshes: %d\n", model->num_meshes);
    fprintf(fp, "Current number of meshes: %d\n", model->cur_num_meshes);
    fprintf(fp, "%s\n", model->directory);
    fprintf(fp, "rotx: %f, roty: %f, rotz: %f, scale: %f\n", model->rotx, model->roty, model->rotz, model->scale);
    
    for(unsigned int i=0; i<model->cur_num_textures; i++)
    {
        fprintf(fp, "texture %d path: %s\n", i, model->textures[i]->path);
    }
    
    for(unsigned int i=0; i<model->num_meshes; i++)
    {
        Mesh* mesh = model->meshes[i];
        fprintf(fp, "----------------------------------------------------------------\n");
        fprintf(fp, "Mesh %d info:\n", i);
        fprintf(fp, "Number of vertices: %d\n", mesh->num_vertices);
        fprintf(fp, "vertices[0] coordinates: %f, %f, %f\n", mesh->vertices[0].Position.x, mesh->vertices[0].Position.y, mesh->vertices[0].Position.z);
        fprintf(fp, "Number of textures: %d\n", mesh->num_textures);
        fprintf(fp, "Number of indices: %d\n", mesh->num_indices);
        fprintf(fp, "VAO: %d, VBO: %d, EBO: %d\n", mesh->VAO, mesh->VBO, mesh->EBO);
        fprintf(fp, "Current number of textures: %d\n", mesh->cur_num_textures);
        fprintf(fp, "Box id: %d\n", mesh->box_id);
        for(unsigned int j=0; j<mesh->num_textures; j++)
        {
            fprintf(fp, "Texture %d: %s%d %s\n", j, model->textures[mesh->textures[j]]->type, model->textures[mesh->textures[j]]->id, model->textures[mesh->textures[j]]->path);
        }
        BoundingBox* box = level->boxes[mesh->box_id];
        fprintf(fp, "Mesh %d bounding box coordinates: %f, %f, %f, %f, %f, %f\n", i, box->v[0].x, box->v[0].y, box->v[0].z, box->v[6].x, box->v[6].y, box->v[6].z);
    }
    fprintf(fp, "----------------------------------------------------------------\n");
    fprintf(fp, "Level info: \n");
    fprintf(fp, "Number of rooms: %d\n", level->num_rooms);
    fprintf(fp, "Number of portals: %d\n", level->num_portals);
    fprintf(fp, "Number of portalBoxes: %d\n", level->num_portalBoxes);
    fprintf(fp, "----------------------------------------------------------------\n");
    for(unsigned int i=0; i<level->num_portals; i++)
    {
        Portal* portal = level->portals[i];
        fprintf(fp, "Portal %d bounds: %f, %f, %f, %f, %f, %f\n", i, portal->v[0].x, portal->v[0].y, portal->v[0].z, portal->v[6].x, portal->v[6].y, portal->v[6].z);
    }
    for(unsigned int i=0; i<level->num_portals; i++)
    {
        BoundingBox* portalbox = level->portalBoxes[i];
        fprintf(fp, "PortalBox %d bounds: %f, %f, %f, %f, %f, %f\n", i, portalbox->v[0].x, portalbox->v[0].y, portalbox->v[0].z, portalbox->v[6].x, portalbox->v[6].y, portalbox->v[6].z);
    }
    fprintf(fp, "----------------------------------------------------------------\n");
    fprintf(fp, "Room info: \n");
    for(unsigned int i=0; i<level->num_rooms; i++)
    {
        Room* room = level->rooms[i];
        fprintf(fp, "Number of meshes: %d\n", room->num_meshes);
        fprintf(fp, "Number of Portals: %d\n", room->num_portals);
        fprintf(fp, "Number of PortalBoxes: %d\n", room->num_portalBoxes);
        fprintf(fp, "Number of neighbors: %d\n", room->num_neighbors);
        BoundingBox* roombox = level->roomBoxes[room->box];
        fprintf(fp, "Room %d bounding box coordinates: %f, %f, %f, %f, %f, %f\n", i, roombox->v[0].x, roombox->v[0].y, roombox->v[0].z, roombox->v[6].x, roombox->v[6].y, roombox->v[6].z);
        //TODO: add portalbox and portal check here
    }

    fclose(fp);

}