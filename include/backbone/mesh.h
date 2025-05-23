#ifndef MESH_INCLUDE
#define MESH_INCLUDE

#include "backbone/objparser.h"

typedef struct
{
    vec3_vert position;
    vec3_vert normal;
    vec2_vert texCoord;

    int indices_count;
    unsigned int *indices;
}Mesh;

typedef struct
{
    unsigned int VAO, VBO, EBO, texture;
}opengl_data;

Mesh *createPackedMesh(Mesh_index m);

opengl_data setUpMesh(Mesh *m_p, const char *texture_path);
void drawMesh(Mesh *m_p, opengl_data obj);




#endif //end of MESH_INCLUDE