#ifndef OBJ_PARSER_INCLUDE
#define OBJ_PARSER_INCLUDE

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct
{
    float v[3];
    float t[3];
}Vert;

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
    Vert *out_verts;
    Vert *out_norms;
    Vert *out_uvs;

    unsigned int *indices;

    ptrdiff_t nverts;
    ptrdiff_t nnorms;
    ptrdiff_t nuvs;

    ptrdiff_t indices_num;
}Mesh;

typedef struct
{
    Vert *out_verts;
    Vert *out_norms;
    Vert *out_uvs;

    ptrdiff_t nverts;
    ptrdiff_t nnorms;
    ptrdiff_t nuvs;

    int n_face;
}Model;

Model loadObjFromFile(const char *path);
Mesh createMeshFromModel(Model m);



#endif // end of OBJ_PARSER_INCLUDE