#ifndef NEBU_3DS_MESH_H
#define NEBU_3DS_MESH_H

#include <lib3ds/file.h>
#include <lib3ds/chunk.h>
#include <lib3ds/io.h>
#include <lib3ds/material.h>
#include <lib3ds/mesh.h>
#include <lib3ds/camera.h>
#include <lib3ds/light.h>
#include <lib3ds/node.h>
#include <lib3ds/vector.h>

#include "nebu_mesh.h"

typedef Lib3dsFile nebu_Mesh_3DSFile;

nebu_Mesh_3DSFile* nebu_Mesh_Load3DSFile(const char *filename);
nebu_Mesh* nebu_Mesh_GetFrom3DSFile(nebu_Mesh_3DSFile *file);
nebu_Mesh* nebu_Mesh_Load3DS(const char *filename);
void nebu_Mesh_Free3DSFile(nebu_Mesh_3DSFile *file);

#endif
