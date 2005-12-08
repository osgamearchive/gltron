#include <stdio.h>
#include <assert.h>

#include "filesystem/path.h"
#include "video/nebu_mesh.h"
#include "video/video_level.h"
#include "video/video.h"

#include "Nebu_scripting.h"
#include "video/nebu_renderer_gl.h"

#include <string.h>

#include "base/nebu_debug_memory.h"

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
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void video_Shader_Cleanup(video_level_shader* shader) {
	glDisable(GL_TEXTURE_2D);
}

int level_LoadTexture() {
	int id;
	char *filename;
	int filter[] = { GL_NEAREST, GL_LINEAR, GL_LINEAR_MIPMAP_NEAREST, 
									 GL_LINEAR_MIPMAP_LINEAR };
	int wrap[] = { GL_CLAMP, GL_CLAMP_TO_EDGE, GL_REPEAT };
	int result;

	// FIXME: error checking

	glGenTextures(1, & id);
	glBindTexture(GL_TEXTURE_2D, id);
	// printf("binding texture %d\n", id);

	scripting_GetValue("file");
	scripting_GetStringResult(& filename);
	loadTexture(filename, GL_RGBA);
	free(filename);

	scripting_GetValue("min_filter");
	scripting_GetIntegerResult(&result);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter[result]);

	scripting_GetValue("mag_filter");
	scripting_GetIntegerResult(&result);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter[result]);
	
	scripting_GetValue("wrap_s");
	scripting_GetIntegerResult(&result);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap[result]);
	
	scripting_GetValue("wrap_t");
	scripting_GetIntegerResult(&result);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap[result]);
	
	return id;
}

void level_LoadShader(video_level_shader *shader) {
	scripting_GetValue("shading");
	scripting_GetValue("lit");
	scripting_GetIntegerResult(& shader->lit);
	scripting_GetValue("textures");
	scripting_GetValue("diffuse");
	
	shader->diffuse_texture_id = level_LoadTexture();

	scripting_Pop(); // diffuse
	scripting_Pop(); // textures
	scripting_Pop(); // shading
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
	level_LoadShader(& l->floor_shader);
	scripting_Pop(); // floor
	
	scripting_GetValue("arena");
	l->arena = loadMesh();
	level_LoadShader(& l->arena_shader);
	scripting_Pop(); // arena
		
	scripting_Pop(); // geometry

	scripting_Pop(); // level;

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

	{
		int nPrimitives, nVertices, vertexformat;

		scripting_GetValue("indices");
		scripting_GetArraySize(&nPrimitives);
		scripting_Pop(); // indices

		scripting_GetValue("vertexformat");
		scripting_GetIntegerResult(&vertexformat);
		scripting_GetValue("vertices");
		scripting_GetArraySize(&nVertices);
		scripting_Pop(); // vertices

		pMesh = nebu_Mesh_Create(vertexformat, nVertices, nPrimitives);
	}

	scripting_GetValue("vertices");
	for(i = 0; i < pMesh->pVB->nVertices; i++) {
		scripting_GetArrayIndex(i + 1);
		if(pMesh->pVB->vertexformat & NEBU_MESH_POSITION) {
			scripting_GetValue("pos");
			scripting_GetValue("x");
			scripting_GetFloatResult( & pMesh->pVB->pVertices[3 * i + 0] );
			scripting_GetValue("y");
			scripting_GetFloatResult( & pMesh->pVB->pVertices[3 * i + 1] );
			scripting_GetValue("z");
			scripting_GetFloatResult( & pMesh->pVB->pVertices[3 * i + 2] );
			scripting_Pop(); // pos
		}
		if(pMesh->pVB->vertexformat & NEBU_MESH_NORMAL) {
			scripting_GetValue("normal");
			scripting_GetValue("x");
			scripting_GetFloatResult( & pMesh->pVB->pNormals[3 * i + 0] );
			scripting_GetValue("y");
			scripting_GetFloatResult( & pMesh->pVB->pNormals[3 * i + 1] );
			scripting_GetValue("z");
			scripting_GetFloatResult( & pMesh->pVB->pNormals[3 * i + 2] );
			scripting_Pop(); // pos
		}
		if(pMesh->pVB->vertexformat & NEBU_MESH_TEXCOORD0) {
			scripting_GetValue("uv");
			scripting_GetValue("u");
			scripting_GetFloatResult( & pMesh->pVB->pTexCoords[0][2 * i + 0] );
			scripting_GetValue("v");
			scripting_GetFloatResult( & pMesh->pVB->pTexCoords[0][2 * i + 1] );
			scripting_Pop(); // uv
		}
		scripting_Pop(); // index i
	}
	scripting_Pop(); // vertices
	
	scripting_GetValue("indices");
	for(i = 0; i < pMesh->pIB->nPrimitives; i++) {
		scripting_GetArrayIndex(i + 1);
		for(j = 0; j < 3; j++)
		{
			scripting_GetArrayIndex(j + 1);
			scripting_GetIntegerResult( & pMesh->pIB->pIndices[3 * i + j] );
		}
		scripting_Pop(); // index i;
	}
	scripting_Pop(); // indices

	return pMesh;
}
