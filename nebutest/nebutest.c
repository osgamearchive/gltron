#include "video/nebu_video_system.h"
#include "video/nebu_mesh_3ds.h"
#include "base/nebu_system.h"

extern void display(void);
extern void idle(void);

nebu_Mesh *pMesh = NULL;

void testMesh(void)
{
	nebu_Mesh_3ds_File *pFile;
	
	pFile = nebu_Mesh_3ds_LoadFile("models/teapot.3ds");
	if(!pFile)
		return;
	pMesh = nebu_Mesh_3ds_GetFromFile(pFile);
	nebu_Mesh_3ds_FreeFile(pFile);
}

int main(int argc, char *argv[])
{
	int window_id = 0;
	nebu_Init();
	nebu_Video_Init();
	nebu_Video_SetWindowMode(0, 0, 800, 600);
	nebu_Video_SetDisplayMode(SYSTEM_32_BIT | SYSTEM_RGBA | SYSTEM_DOUBLE | SYSTEM_STENCIL | SYSTEM_DEPTH);
	window_id = nebu_Video_Create("nebutest");

	// test: mesh handling
	testMesh();

	nebu_System_SetCallback_Display(display);
	nebu_System_SetCallback_Idle(idle);
	nebu_System_MainLoop();
	nebu_Video_Destroy(window_id);

	return 0;
}
