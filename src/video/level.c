#include <stdlib.h>
#include <stdio.h>

#include "SDL_opengl.h"

#include "filesystem/path.h"
#include "video/nebu_mesh.h"
#include "video/video_level.h"
#include "video/video.h"

#include "Nebu_scripting.h"

nebu_Mesh* loadMesh(void);

void video_FreeLevel(video_level *l) {
	nebu_Mesh_Free(l->floor);
	nebu_Mesh_Free(l->arena);
	glDeleteTextures(1, & l->floor_shader.diffuse_texture_id);
	glDeleteTextures(1, & l->arena_shader.diffuse_texture_id);
	free(l);
}

void video_ScaleLevel(video_level *l, float fSize)
{
	nebu_Mesh_Scale(l->floor, fSize);
	nebu_Mesh_Scale(l->arena, fSize);
}

void video_Shader_Setup(video_level_shader* shader) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, shader->diffuse_texture_id);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

void video_Shader_Cleanup(video_level_shader* shader) {
	glDisable(GL_TEXTURE_2D);
}

void loadShader(video_level_shader *shader) {
	scripting_GetValue("shading");
	scripting_GetValue("lit");
	scripting_GetIntegerResult(& shader->lit);
	scripting_GetValue("textures");
	scripting_GetValue("diffuse");
	
	// FIXME: use a general load texture method, and handle stuff
	// like filtering, wrap/clamp, etc.
	{
		char *filename;
		scripting_GetStringResult(& filename);

		glGenTextures(1, & shader->diffuse_texture_id);
		glBindTexture(GL_TEXTURE_2D, shader->diffuse_texture_id);
		printf("binding texture %d\n", shader->diffuse_texture_id);
		loadTexture(filename, GL_RGBA);
		free(filename);
	}
	scripting_PopTable(); // textures
	scripting_PopTable(); // shader
}

video_level* video_CreateLevel(void) {
	video_level *l;

	l = malloc( sizeof(video_level) );

	scripting_GetGlobal("level", NULL);
	// get scalability flag
	scripting_GetValue("scalable");
	scripting_GetIntegerResult(& l->scalable);

	scripting_GetValue("geometry");

	// get floor & arena meshes
	scripting_GetValue("floor");
	l->floor = loadMesh();
	loadShader(& l->floor_shader);
	scripting_PopTable(); // floor
	
	scripting_GetValue("arena");
	l->arena = loadMesh();
	loadShader(& l->arena_shader);
	scripting_PopTable(); // arena
		
	scripting_PopTable(); // geometry

	scripting_PopTable(); // level;

	return l;
}

enum {
	MESH_POSITION = 1,
	MESH_NORMAL = 2,
	MESH_TEXCOORD0 = 4
};

enum {
	V_POS = 0,
	V_NORMAL,
	V_TEXCOORD0
};
	

nebu_Mesh* loadMesh(void) {
	nebu_Mesh *pMesh;
	int i, j;
	
	pMesh = (nebu_Mesh*) malloc( sizeof(nebu_Mesh) );
	
	scripting_GetValue("vertexformat");
	scripting_GetIntegerResult(& pMesh->vertexformat);
	scripting_GetValue("vertices");
	scripting_GetArraySize(& pMesh->nVertices);

	if(pMesh->vertexformat & NEBU_MESH_POSITION)
		pMesh->pVertices = malloc( pMesh->nVertices * 3 * sizeof(float) );
	if(pMesh->vertexformat & NEBU_MESH_NORMAL)
		pMesh->pNormals = malloc( pMesh->nVertices * 3 * sizeof(float) );
	if(pMesh->vertexformat & NEBU_MESH_TEXCOORD)
		pMesh->pTexCoords = malloc( pMesh->nVertices * 2 * sizeof(float) );

	for(i = 0; i < pMesh->nVertices; i++) {
		scripting_GetArrayIndex(i + 1);
		if(pMesh->vertexformat & NEBU_MESH_POSITION) {
			scripting_GetValue("pos");
			scripting_GetValue("x");
			scripting_GetFloatResult( & pMesh->pVertices[3 * i + 0] );
			scripting_GetValue("y");
			scripting_GetFloatResult( & pMesh->pVertices[3 * i + 1] );
			scripting_GetValue("z");
			scripting_GetFloatResult( & pMesh->pVertices[3 * i + 2] );
			scripting_PopTable(); // pos
		}
		if(pMesh->vertexformat & NEBU_MESH_NORMAL) {
			scripting_GetValue("normal");
			scripting_GetValue("x");
			scripting_GetFloatResult( & pMesh->pNormals[3 * i + 0] );
			scripting_GetValue("y");
			scripting_GetFloatResult( & pMesh->pNormals[3 * i + 1] );
			scripting_GetValue("z");
			scripting_GetFloatResult( & pMesh->pNormals[3 * i + 2] );
			scripting_PopTable(); // pos
		}
		if(pMesh->vertexformat & NEBU_MESH_TEXCOORD) {
			scripting_GetValue("uv");
			scripting_GetValue("u");
			scripting_GetFloatResult( & pMesh->pTexCoords[2 * i + 0] );
			scripting_GetValue("v");
			scripting_GetFloatResult( & pMesh->pTexCoords[2 * i + 1] );
			scripting_PopTable(); // uv
		}
		scripting_PopTable(); // index i
	}
	scripting_PopTable(); // vertices
	
	scripting_GetValue("indices");
	scripting_GetArraySize(& pMesh->nTriangles);
	pMesh->pTriangles = malloc( pMesh->nTriangles * 3 * sizeof(int) );
	for(i = 0; i < pMesh->nTriangles; i++) {
		scripting_GetArrayIndex(i + 1);
		for(j = 0; j < 3; j++)
		{
			scripting_GetArrayIndex(j + 1);
			scripting_GetIntegerResult( & pMesh->pTriangles[3 * i + j] );
		}
		scripting_PopTable(); // index i;
	}
	scripting_PopTable(); // indices

	return pMesh;
}
