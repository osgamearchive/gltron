#include "video/nebu_video_system.h"
#include "video/nebu_mesh_3ds.h"
#include "video/nebu_scene.h"
#include "video/nebu_camera.h"
#include "video/nebu_renderer_gl.h"
#include "base/nebu_math.h"
#include "base/nebu_system.h"
#include "base/nebu_vector.h"
#include "base/nebu_math.h"
#include "input/nebu_input_system.h"

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

void keyboard(int state, int key, int x, int y)
{
	if(key == 27)
	{
		nebu_System_ExitLoop(0);
	}
}

void mouseMotion(int x, int y)
{
	int dx, dy;
	nebu_Input_Mouse_GetDelta(&dx, &dy);
	if(dx || dy)
	{
		nebu_Camera_Rotate(pScene->pCamera,
			NEBU_CAMERA_ROTATE_AROUND_EYE |
			NEBU_CAMERA_FIXED_UP,
			(float)dx, (float)dy);
		nebu_Input_Mouse_WarpToOrigin();
		nebu_System_PostRedisplay();
	}
}

void solidShaderStart(void) {
	glColor3f(1,1,1);
}

void wireFrameSetup(void)
{
	glColor3f(1,0,0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}
void wireFrameCleaup(void)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void testSolidShader(void)
{
	pShader = nebu_Shader_Create();
	pShader->setup = solidShaderStart;
}


void testWireFrameShader(void)
{
	pShader = nebu_Shader_Create();
	pShader->setup = wireFrameSetup;
	pShader->cleanup = wireFrameCleaup;
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
		eye.v[0] = 0; eye.v[1] = 0; eye.v[2] = 100;
		up.v[0] = 0; up.v[1] = 1; up.v[2] = 0;
		lookat.v[0] = 0; lookat.v[1] = 0; lookat.v[2] = 0;
		nebu_Camera_SetupEyeUpLookAt(pCamera, &eye, &up, &lookat);
		nebu_Scene_SetCamera(pScene, pCamera);		
	}
}

void doPerspective(float fov, float ratio, float znear, float zfar) {
  float top;
  float left;

  top = tanf( fov * PI / 360.0 ) * znear;
  left = - top * ratio;
  glFrustum(left, -left, -top, top, znear, zfar);
}

int main(int argc, char *argv[])
{
	int window_id = 0;
	nebu_Init();
	nebu_Video_Init();
	nebu_Video_SetWindowMode(0, 0, 800, 600);
	nebu_Video_SetDisplayMode(SYSTEM_32_BIT | SYSTEM_RGBA | SYSTEM_DOUBLE | SYSTEM_STENCIL | SYSTEM_DEPTH);
	window_id = nebu_Video_Create("nebutest");

	nebu_Input_Mouse_WarpToOrigin();
	nebu_Input_Grab();

	glViewport(0,0,800,600);
	glMatrixMode(GL_PROJECTION);
	doPerspective(45, 4.0f/3.0f, 0.1f, 1000);
	// setup emtpy scene
	setupScene();

	// test: mesh handling
	testMesh();
	// test: wire frame shading
	testWireFrameShader();

	// add both to scene
	nebu_Scene_AddObject(pScene, pShader, pMesh);
	nebu_System_PostRedisplay();

	nebu_System_SetCallback_Display(display);
	nebu_System_SetCallback_Key(keyboard);
	nebu_System_SetCallback_MouseMotion(mouseMotion);
	nebu_System_SetCallback_Idle(idle);
	
	nebu_System_MainLoop();
	nebu_Video_Destroy(window_id);

	return 0;
}
