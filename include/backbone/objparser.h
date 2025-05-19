#ifndef OBJ_PARSER_INCLUDE
#define OBJ_PARSER_INCLUDE

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct
{
    float v[3];
}Vert;

typedef struct
{
    int32_t v[3];
    int32_t n[3];
    int32_t t[2];
}Face;

typedef struct
{
    float *verts;
    float *norms;
    float *texs;

    unsigned int *pos_indices;
    ptrdiff_t nverts;
    ptrdiff_t nnorms;
    ptrdiff_t ntexs;
    ptrdiff_t nfaces;
}Model;

Model loadObjFromFile(const char *path);



#endif // end of OBJ_PARSER_INCLUDE