#ifndef NEBU_SCENE_H
#define NEBU_SCENE_H

#include "video/nebu_mesh.h"
#include "video/nebu_camera.h"

typedef struct {
	void *foo;
} nebu_Scene;

typedef struct {
	void (*setup)(void);
} nebu_Shader;

void nebu_Scene_AddMesh(nebu_Scene *pScene, nebu_Mesh* pMesh);
void nebu_Scene_AddObject(nebu_Scene *pScene, nebu_Shader *pShader, nebu_Mesh *pMesh);
nebu_Scene* nebu_Scene_Create(void);
void nebu_Scene_SetCamera(nebu_Scene *pScene, nebu_Camera *pCamera);
void nebu_Scene_Draw(nebu_Scene *pScene);

nebu_Shader* nebu_Shader_Create(void);

#endif