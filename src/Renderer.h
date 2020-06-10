#ifndef RENDERER_H_
#define RENDERER_H_

struct Level;
struct Model;
struct Shader;
struct Camera;
struct Room;

const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 5000.f;

void initiateRenderer();

void prepareRenderer();

void render(Level* level, Model* model, Shader* shader, Camera* camera);

#endif

