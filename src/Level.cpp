#include <stdio.h>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Model.h"
#include "Level.h"


bool boxInsideBox(BoundingBox* bigBox, BoundingBox* smallBox)
{
    unsigned int out = 0;
    if(bigBox->v[0].x <= smallBox->v[0].x)
        out++;
    if(bigBox->v[0].y <= smallBox->v[0].y)
        out++;
    if(bigBox->v[0].z <= smallBox->v[0].z)
        out++;
    if(bigBox->v[6].x >= smallBox->v[6].x)
        out++;
    if(bigBox->v[6].y >= smallBox->v[6].y)
        out++;
    if(bigBox->v[6].z >= smallBox->v[6].z)
        out++;
    if(out == 6)
        return true;
    else return false;
}

bool pointInsideBox(BoundingBox* box, glm::vec3 point)
{
    int out = 0;

	if (box->v[0].x <= point.x)
		out++;
	if (box->v[0].y <= point.y)
		out++;
	if (box->v[0].z <= point.z)
		out++;
	if (box->v[6].x >= point.x)
		out++;
	if (box->v[6].y >= point.y)
		out++;
	if (box->v[6].z >= point.z)
		out++;
	if (out == 6) return true;
		else return false;
}

bool checkBoxVisibility(BoundingBox* box, Frustum* frustum, glm::mat4 comboMatrix)
{
    float left = 10000000.0f;
	float right = -10000000.0f;
	float bottom = 10000000.0f;
	float top = -10000000.0f;
	float near = 10000000.0f;
	float far = -10000000.0f;

    for(unsigned int i=0; i<8; i++)
    {
        glm::vec4 vec = comboMatrix*(glm::vec4(box->v[i], 1.0f));
        vec.x /= vec.w;
        vec.y /= vec.w;
        vec.z /= vec.w;
        if(vec.x < left) left = vec.x;
        if(vec.x > right) right = vec.x;
        if(vec.y < bottom) bottom = vec.y;
        if(vec.y > top) top = vec.y;
        if(vec.z < near) near = vec.z;
        if(vec.z > far) far = vec.z;
    }

    bool colX = (right >= frustum->left)&&(frustum->right >= left);
    bool colY = (top >= frustum->bottom)&&(frustum->top >= bottom);
    //bool colZ = (far >= -1.0f) && (1.0f >= near);
    return colX&&colY;//&&colZ;    
}

bool checkPortalVisibility(Portal* portal, Frustum* frustum, glm::mat4 comboMatrix)
{
    float left = 10000000.0f;
	float right = -10000000.0f;
	float bottom = 10000000.0f;
	float top = -10000000.0f;
	float near = 10000000.0f;
	float far = -10000000.0f;

    for(unsigned int i=0; i<4; i++)
    {
        glm::vec4 vec = comboMatrix*(glm::vec4(portal->v[i], 1.0f));
        vec.x /= vec.w;
        vec.y /= vec.w;
        vec.z /= vec.w;
        if(vec.x < left) left = vec.x;
        if(vec.x > right) right = vec.x;
        if(vec.y < bottom) bottom = vec.y;
        if(vec.y > top) top = vec.y;
        if(vec.z < near) near = vec.z;
        if(vec.z > far) far = vec.z;
    }

    bool colX = (right >= frustum->left)&&(frustum->right >= left);
    bool colY = (top >= frustum->bottom)&&(frustum->top >= bottom);
    //bool colZ = (far >= -1.0f) && (1.0f >= near);
    return colX&&colY;//&&colZ;    
}

Frustum clipFrustum(Frustum* frustum, Portal* portal, glm::mat4 comboMatrix)
{
	float left = 10000000.0f;
	float right = -10000000.0f;
	float bottom = 10000000.0f;
	float top = -10000000.0f;

    for (unsigned int i=0; i<4; i++)
    {
        glm::vec4 vec = comboMatrix*(glm::vec4(portal->v[i], 1.0f));
        vec.x /= vec.w;
        vec.y /= vec.w;
        vec.z /= vec.w;
        if(vec.x < left) left = vec.x;
        if(vec.x > right) right = vec.x;
        if(vec.y < bottom) bottom = vec.y;
        if(vec.y > top) top = vec.y;
    }

    Frustum tempFrustum = {frustum->left, frustum->right, frustum->bottom, frustum->top};

    if(right > frustum->left && right < frustum->right)
        tempFrustum.right = right;
    if(left > frustum->left && left < frustum->right)
        tempFrustum.left = left;
    if(top > frustum->bottom && top < frustum->bottom)
        tempFrustum.top = top;
    if(bottom > frustum->bottom && bottom < frustum->top)
        tempFrustum.bottom = bottom;
    return tempFrustum;
}

void createPortalBoxes(Level* level)
{
    level->portalBoxes = (BoundingBox**)malloc(level->num_portalBoxes*sizeof(BoundingBox*));

    BoundingBox* portalBox12 = createBoundingBox(glm::vec3(2900.0f, -500.0f, -3700.0f),
                                                 glm::vec3(3400.0f, 0.0f, -3500.0f));
    level->portalBoxes[0] = portalBox12;

    BoundingBox* portalBox23 = createBoundingBox(glm::vec3(2740.0f, -500.0f, -2100.0f),
                                                 glm::vec3(2940.0f, 0.0f, -1560.0f));
    level->portalBoxes[1] = portalBox23;

    BoundingBox* portalBox25 = createBoundingBox(glm::vec3(3290.0f, -490.0f, -580.0f),
                                                 glm::vec3(3650.0f, -10.0f, -100.0f));
    level->portalBoxes[2] = portalBox25;

    BoundingBox* portalBox34 = createBoundingBox(glm::vec3(1400.0f, -500.0f, -1700.0f),
                                                 glm::vec3(1900.0f, 0.0f, -1450.0f));
    level->portalBoxes[3] = portalBox34;
    
    BoundingBox* portalBox46 = createBoundingBox(glm::vec3(1330.0f, -320.0f, -630.0f),
                                                 glm::vec3(2060.0f, 540.0f, -170.0f));
    level->portalBoxes[4] = portalBox46;

    BoundingBox* portalBox67 = createBoundingBox(glm::vec3(250.0f, -320.0f, -1150.0f),
                                                 glm::vec3(670.0f, -20.0f, -840.0f));
    level->portalBoxes[5] = portalBox67;
}

void createPortals(Level* level)
{
    level->portals = (Portal**)malloc(level->num_portals*sizeof(Portal*));

    Portal* portal12 = createPortal(glm::vec3(3400.0f, -500.0f, -3600.0f),
                                   glm::vec3(3400.0f, 0.0f, -3600.0f),
                                   glm::vec3(2900.0f, -500.0f, -3600.0f),
                                   glm::vec3(2900.0f, 0.0f, -3600.0f));
    level->portals[0] = portal12;

    Portal* portal23 = createPortal(glm::vec3(2910.0f, -600.0f, -1520.0f),
                                   glm::vec3(2910.0f, 0.0f, -1520.0f),
                                   glm::vec3(2910.0f, -600.0f, -2100.0f),
                                   glm::vec3(2910.0f, 0.0f, -2100.0f));
    level->portals[1] = portal23;

    Portal* portal25 = createPortal(glm::vec3(3390.0f, -510.0f, -590.0f),
                                   glm::vec3(3390.0f, -510.0f, -110.0f),
                                   glm::vec3(3390.0f, 0.0f, -110.0f),
                                   glm::vec3(3390.0f, 0.0f, -590.0f));
    level->portals[2] = portal25;

    Portal* portal34 = createPortal(glm::vec3(1900.0f, -490.0f, -1600.0f),
                                    glm::vec3(1380.0f, -490.0f, -1600.0f),
                                    glm::vec3(1380.0f, -10.0f, -1600.0f),
                                    glm::vec3(1900.0f, -10.0f, -1600.0f));
    level->portals[3] = portal34;

    Portal* portal46 = createPortal(glm::vec3(1940.0f, 20.0f, -390.0f),
                                    glm::vec3(1360.0f, 20.0f, -390.0f),
                                    glm::vec3(1360.0f, 570.0f, -390.0f),
                                    glm::vec3(1940.0f, 570.0f, -390.0f));
    level->portals[4] = portal46;

    Portal* portal67 = createPortal(glm::vec3(280.0f, -30.0f, -1020.0f),
                                    glm::vec3(630.0f, -30.0f, -1020.0f),
                                    glm::vec3(630.0f, 340.0f, -1020.0f),
                                    glm::vec3(280.0f, 340.0f, -1020.0f));
    level->portals[5] = portal67;
}

void createRoomBoxes(Level* level)
{
    level->roomBoxes = (BoundingBox**)malloc(level->num_rooms*sizeof(BoundingBox*));

    BoundingBox* room1Box = createBoundingBox(glm::vec3(2260.0f, -900.0f, -5580.0f), 
                                              glm::vec3(4000.0f, 400.0f, -3450.0f));
    level->roomBoxes[0] = room1Box;

    BoundingBox* room2Box = createBoundingBox(glm::vec3(2740.0f, -600.0f, -3760.0f), 
                                              glm::vec3(3540.0f, 0.0f, 50.0f));
    level->roomBoxes[1] = room2Box;

    BoundingBox* room3Box = createBoundingBox(glm::vec3(1270.0f, -600.0f, -2250.0f), 
                                              glm::vec3(2980.0f, 60.0f, -1430.0f));
    level->roomBoxes[2] = room3Box;

    BoundingBox* room4Box = createBoundingBox(glm::vec3(1260.0f, -700.0f, -1690.0f),
                                              glm::vec3(2050.0f, 810.0f, -310.0f));
    level->roomBoxes[3] = room4Box;

    BoundingBox* room5Box = createBoundingBox(glm::vec3(3330.0f, -600.0f, -1140.0f),
                                              glm::vec3(8470.0f, 400.0f, 250.0f));
    level->roomBoxes[4] = room5Box;

    BoundingBox* room6Box = createBoundingBox(glm::vec3(-1800.0f, -20.0f, -1420.0f),
                                              glm::vec3(3800.0f, 760.0f, 1650.0f));
    level->roomBoxes[5] = room6Box;

    BoundingBox* room7Box = createBoundingBox(glm::vec3(-1760.0f, -50.0f, -2920.0f),
                                              glm::vec3(1450.0f, 640.0f, -750.0f));
    level->roomBoxes[6] = room7Box;
}

void createRooms(Level* level)
{
    Room** rooms = (Room**)malloc(level->num_rooms*sizeof(Room*));
    level->rooms = rooms;
    //Room1
    Room* room1 = (Room*)malloc(sizeof(Room));

    room1->box = 0;
    room1->num_portals = 1;
    room1->num_portalBoxes = 1;
    room1->num_neighbors = 1;

    room1->portalBoxes = (int*)malloc(room1->num_portalBoxes*sizeof(int));
    room1->portalBoxes[0] = 0;
    room1->portals = (int*)malloc(room1->num_portals*sizeof(int));
    room1->portals[0] = 0;
    room1->neighbors = (int*)malloc(room1->num_neighbors*sizeof(int));
    room1->neighbors[0] = 1;
    room1->isDrawn = false;
    level->rooms[0] = room1;

    //Room2
    Room* room2 = (Room*)malloc(sizeof(Room));

    room2->box = 1;
    room2->num_portals = 3;
    room2->num_portalBoxes = 3;
    room2->num_neighbors = 3;

    room2->portalBoxes = (int*)malloc(room2->num_portalBoxes*sizeof(int));
    room2->portalBoxes[0] = 0;
    room2->portalBoxes[1] = 1;
    room2->portalBoxes[2] = 2;
    room2->portals = (int*)malloc(room2->num_portals*sizeof(int));
    room2->portals[0] = 0;
    room2->portals[1] = 1;
    room2->portals[2] = 2;
    room2->neighbors = (int*)malloc(room2->num_neighbors*sizeof(int));
    room2->neighbors[0] = 0;
    room2->neighbors[1] = 2;
    room2->neighbors[2] = 4;
    room2->isDrawn = false;
    level->rooms[1] = room2;

    //Room3
    Room* room3 = (Room*)malloc(sizeof(Room));

    room3->box = 2;
    room3->num_portals = 2;
    room3->num_portalBoxes = 2;
    room3->num_neighbors = 2;

    room3->portalBoxes = (int*)malloc(room3->num_portalBoxes*sizeof(int));
    room3->portalBoxes[0] = 1;
    room3->portalBoxes[1] = 3;
    room3->portals = (int*)malloc(room3->num_portals*sizeof(int));
    room3->portals[0] = 1;
    room3->portals[1] = 3;
    room3->neighbors = (int*)malloc(room3->num_neighbors*sizeof(int));
    room3->neighbors[0] = 1;
    room3->neighbors[1] = 3;
    room3->isDrawn = false;
    level->rooms[2] = room3;
    //Room4
    Room* room4 = (Room*)malloc(sizeof(Room));

    room4->box = 3;
    room4->num_portals = 2;
    room4->num_portalBoxes = 2;
    room4->num_neighbors = 2;

    room4->portalBoxes = (int*)malloc(room4->num_portalBoxes*sizeof(int));
    room4->portalBoxes[0] = 3;
    room4->portalBoxes[1] = 4;
    room4->portals = (int*)malloc(room4->num_portals*sizeof(int));
    room4->portals[0] = 3;
    room4->portals[1] = 4;
    room4->neighbors = (int*)malloc(room4->num_neighbors*sizeof(int));
    room4->neighbors[0] = 2;
    room4->neighbors[1] = 5;
    room4->isDrawn = false;
    level->rooms[3] = room4;
    //Room5
    Room* room5 = (Room*)malloc(sizeof(Room));

    room5->box = 4;
    room5->num_portals = 1;
    room5->num_portalBoxes = 1;
    room5->num_neighbors = 1;

    room5->portalBoxes = (int*)malloc(room5->num_portalBoxes*sizeof(int));
    room5->portalBoxes[0] = 2;
    room5->portals = (int*)malloc(room5->num_portals*sizeof(int));
    room5->portals[0] = 2;
    room5->neighbors = (int*)malloc(room5->num_neighbors*sizeof(int));
    room5->neighbors[0] = 1;
    room5->isDrawn = false;
    level->rooms[4] = room5;
    //Room6
    Room* room6 = (Room*)malloc(sizeof(Room));

    room6->box = 5;
    room6->num_portals = 2;
    room6->num_portalBoxes = 2;
    room6->num_neighbors = 2;

    room6->portalBoxes = (int*)malloc(room6->num_portalBoxes*sizeof(int));
    room6->portalBoxes[0] = 4;
    room6->portalBoxes[1] = 5;
    room6->portals = (int*)malloc(room6->num_portals*sizeof(int));
    room6->portals[0] = 4;
    room6->portals[1] = 5;
    room6->neighbors = (int*)malloc(room6->num_neighbors*sizeof(int));
    room6->neighbors[0] = 3;
    room6->neighbors[1] = 6;
    room6->isDrawn = false;
    level->rooms[5] = room6;
    //Room7
    Room* room7 = (Room*)malloc(sizeof(Room));

    room7->box = 6;
    room7->num_portals = 1;
    room7->num_portalBoxes = 1;
    room7->num_neighbors = 1;

    room7->portalBoxes = (int*)malloc(room7->num_portalBoxes*sizeof(int));
    room7->portalBoxes[0] = 5;
    room7->portals = (int*)malloc(room7->num_portals*sizeof(int));
    room7->portals[0] = 5;
    room7->neighbors = (int*)malloc(room7->num_neighbors*sizeof(int));
    room7->neighbors[0] = 5;
    room7->isDrawn = false;
    level->rooms[6] = room7;
}

void readLevelConfig(Level* level, Model* model, const char* path)
{
    level->boxes = (BoundingBox**)malloc(model->num_meshes*sizeof(BoundingBox*));
    level->num_to_draw = 0;
    level->meshesToBeDrawn = (int*)malloc(model->num_meshes*sizeof(int)); //worst case is taken (max number of meshes that can be drawn) to avoid dynamically allocating this every frame. 
    FILE* fp = fopen(path, "r");
    int bufferLength = 255;
    char buffer[255];
    int currentRoom = 0;
    int lineCount = 0;

    if(fp==NULL)
    {
        printf("Can not read file at location: %s\n", path);
        return;
    }
    while(fgets(buffer, bufferLength, fp))
    {
        if(buffer[0] == 'R' )
        {
            char subbuff[4];
            memcpy(subbuff, &buffer[3], 3);
            subbuff[3] = '\0';
            int num_meshes = atoi(subbuff);
            level->rooms[currentRoom]->num_meshes = num_meshes;
            level->rooms[currentRoom]->meshes = (int*)malloc(num_meshes*sizeof(int));
            for(int i=0; i<num_meshes; i++)
            {
                if(fgets(buffer, bufferLength, fp))
                {
                    int mesh_id = atoi(buffer);
                    level->rooms[currentRoom]->meshes[i] = mesh_id;
                }
            }
            currentRoom++;
        }
        else if(buffer[0]=='m')
        {
            char * pEnd;
            float minx, miny, minz, maxx, maxy, maxz;
            if(lineCount < 10)
                minx= strtof (&buffer[7],&pEnd);
            else if(lineCount <100)
                minx = strtof(&buffer[8], &pEnd);
            else minx = strtof(&buffer[9], &pEnd);
            miny = strtof (pEnd,&pEnd);
            minz = strtof (pEnd,&pEnd);
            maxx = strtof (pEnd,&pEnd);
            maxy = strtof (pEnd,&pEnd);
            maxz = strtof (pEnd,&pEnd);
            glm::vec3 v1 = glm::vec3(minx, miny, minz);
            glm::vec3 v7 = glm::vec3(maxx, maxy, maxz);
            level->boxes[lineCount] = createBoundingBox(v1, v7);
            lineCount++;
        }
    }
    fclose(fp);
}
void printLevel(Level* level)
{
    for(int i=0; i<level->num_rooms; i++)
    {
        int num_meshes = level->rooms[i]->num_meshes;
        int num_portals = level->rooms[i]->num_portals;
        int num_portalBoxes = level->rooms[i]->num_portalBoxes;
        int num_neighbors = level->rooms[i]->num_neighbors;

        printf("Room %d has %d meshes, %d portals, %d portal boxes and %d neighbors\n", i, num_meshes, num_portals, num_portalBoxes, num_neighbors);
        printf("Below are the meshes of room %d:\n", i);
        
        for(int j = 0; j<num_meshes; j++)
        {
            int mesh_id = level->rooms[i]->meshes[j];
            printf("%d\n", mesh_id);
        }
    }
}
Level* createLevel(const char* path, Model* model)
{
    Level* level = (Level*)malloc(sizeof(Level));

    level->num_rooms = 7;
    level->num_portalBoxes = 6;
    createPortalBoxes(level);
    level->num_portals = 6;
    createPortals(level);
    createRoomBoxes(level);
    createRooms(level);
    readLevelConfig(level, model, path);
    
    return level;
}

void processRoom(Level* level, Room* room, Frustum* frustum, Camera* camera, glm::mat4 comboMatrix)
{
    room->isDrawn = true;

    for(unsigned int i=0; i < room->num_meshes; i++)
    {
        if(checkBoxVisibility(level->boxes[room->meshes[i]], frustum, comboMatrix))
        {
            level->meshesToBeDrawn[level->num_to_draw] = room->meshes[i];
            level->num_to_draw++; 
        }
    }

    for(unsigned int i=0; i<room->num_portals; i++) //need to do two separate loops if num_portals!=num_portalBoxes
    {
        if(pointInsideBox(level->portalBoxes[room->portalBoxes[i]], camera->Position))
        {
            if(level->rooms[room->neighbors[i]]->isDrawn == false)
                processRoom(level, level->rooms[room->neighbors[i]], frustum, camera, comboMatrix);
        }
        else if(checkPortalVisibility(level->portals[room->portals[i]], frustum, comboMatrix))
        {
            Frustum clippedFrustum = clipFrustum(frustum, level->portals[room->portals[i]], comboMatrix);
            if(level->rooms[room->neighbors[i]]->isDrawn == false)
                processRoom(level, level->rooms[room->neighbors[i]], &clippedFrustum, camera, comboMatrix);
        }
    }
}

void resetLevel(Level* level)
{
    for(unsigned int i=0; i<level->num_rooms; i++)
    {
        level->rooms[i]->isDrawn = false;
    }
    memset(level->meshesToBeDrawn, 0, level->num_to_draw*sizeof(unsigned int));
    level->num_to_draw = 0;
}