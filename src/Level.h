#ifndef LEVEL_H_
#define LEVEL_H_

#include "Model.h"

struct Room
{
    int* meshes; //file
    int num_meshes; //file
    int cur_num_meshes; //??necessary??

    int box; //by hand

    int* portals;   //by hand
    int num_portals; //by hand

    int* portalBoxes; //by hand
    int num_portalBoxes; //by hand

    int* neighbors; //by hand
    int num_neighbors;
    bool isDrawn;
};

struct Level
{
    Room** rooms;
    Portal** portals;
    BoundingBox** portalBoxes;
    BoundingBox** boxes;
    BoundingBox** roomBoxes;
    
    int num_rooms;
    int num_portals;
    int num_portalBoxes;

    int* meshesToBeDrawn;
    int num_to_draw;
};

bool boxInsideBox(BoundingBox* bigBox, BoundingBox* smallBox);

bool pointInsideBox(BoundingBox* box, glm::vec3 point);

bool checkBoxVisibility(BoundingBox* box, Frustum* frustum, glm::mat4 comboMatrix);

bool checkPortalVisibility(Portal* portal, Frustum* frustum, glm::mat4 comboMatrix);

Frustum clipFrustum(Frustum* frustum, Portal* portal, glm::mat4 comboMatrix);

void createPortalBoxes(Level* level);

void createPortals(Level* level);

void createRoomBoxes(Level* level);

void createRooms(Level* level);

void readLevelConfig(Level* level, Model* model, const char* path);

void printLevel(Level* level);

Level* createLevel(const char* path, Model* model);

void processRoom(Level* level, Room* room, Frustum* frustum, Camera* camera, glm::mat4 comboMatrix);

void resetLevel(Level* level);

#endif