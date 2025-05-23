#ifndef OBJ_PARSER_INCLUDE
#define OBJ_PARSER_INCLUDE

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct
{
    float v[3];
}vec3_vert;

typedef struct
{
    float v[2];
}vec2_vert;

typedef struct
{
    int32_t v[3];
    int32_t n[3];
    int32_t t[3];
}Face;


typedef struct
{
    float v, vt, vn;
}packed_vertex;

typedef struct
{
    vec3_vert *out_verts;
    vec3_vert *out_norms;
    vec2_vert *out_uvs;

    unsigned int *indices;

    ptrdiff_t nverts;
    ptrdiff_t nnorms;
    ptrdiff_t nuvs;

    ptrdiff_t indices_num;
}Mesh;

typedef struct
{
    vec3_vert *out_verts;
    vec3_vert *out_norms;
    vec2_vert *out_uvs;

    ptrdiff_t nverts;
    ptrdiff_t nnorms;
    ptrdiff_t nuvs;

    int n_face;
}Model;

Model loadObjFromFile(const char *path);
Mesh createMeshFromModel(Model m);



#endif // end of OBJ_PARSER_INCLUDE