#include "video/nebu_video_system.h"
#include "video/nebu_mesh_3ds.h"
#include "video/nebu_scene.h"
#include "video/nebu_camera.h"
#include "base/nebu_system.h"
#include "base/nebu_vector.h"

#include "GL/glew.h"

extern void display(void);
extern void idle(void);

nebu_Mesh *pMesh = NULL;
nebu_Scene *pScene = NULL;
nebu_Shader *pShader = NULL;

void testMesh(void)
{
	nebu_Mesh_3ds_File *pFile;
	
	pFile = nebu_Mesh_3ds_LoadFile("models/teapot.3ds");
	if(!pFile)
		return;
	pMesh = nebu_Mesh_3ds_GetFromFile(pFile);
	nebu_Mesh_3ds_FreeFile(pFile);

	nebu_Scene_AddMesh(pScene, pMesh);
}

void solidShaderStart(void) {
	glColor3f(1,1,1);
}

void testSolidShader(void)
{
	pShader = nebu_Shader_Create();
	pShader->setup = solidShaderStart;
}

void setupScene(void)
{
	{
		pScene = nebu_Scene_Create();
	}

	{
		nebu_Camera *pCamera;
		vec3 eye;
		vec3 up;
		vec3 lookat;

		pCamera = nebu_Camera_Create();
		eye.v[0] = 2; eye.v[1] = 1; eye.v[2] = 7;
		up.v[0] = 0; up.v[1] = 1; up.v[2] = 0;
		lookat.v[0] = 0; lookat.v[1] = 0; lookat.v[2] = 0;
		nebu_Camera_SetupEyeUpLookAt(pCamera, &eye, &up, &lookat);
		nebu_Scene_SetCamera(pScene, pCamera);		
	}
}

int main(int argc, char *argv[])
{
	int window_id = 0;
	nebu_Init();
	nebu_Video_Init();
	nebu_Video_SetWindowMode(0, 0, 800, 600);
	nebu_Video_SetDisplayMode(SYSTEM_32_BIT | SYSTEM_RGBA | SYSTEM_DOUBLE | SYSTEM_STENCIL | SYSTEM_DEPTH);
	window_id = nebu_Video_Create("nebutest");

	// setup emtpy scene
	setupScene();

	// test: mesh handling
	testMesh();
	// test: solid shading
	testSolidShader();

	// add both to scene
	nebu_Scene_AddObject(pScene, pShader, pMesh);

	nebu_System_SetCallback_Display(display);
	nebu_System_SetCallback_Idle(idle);
	nebu_System_MainLoop();
	nebu_Video_Destroy(window_id);

	return 0;
}
