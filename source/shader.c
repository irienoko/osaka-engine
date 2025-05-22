#include "backbone/shader.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


unsigned int loadShader(const char *path, unsigned int program)
{
    printf("loading %s\n", path);
    char *f_content = NULL;
    int fsize = 0;

    FILE *fp;

    fp = fopen(path, "rb");

    if(fp)
    {
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        rewind(fp);

        f_content = (char*)calloc(fsize, sizeof(char));
        fread(f_content, fsize, 1, fp);

        fclose(fp);
    }

    unsigned int shader_type;
    shader_type = glCreateShader(program);
    glShaderSource(shader_type, 1, (const char* const*)&f_content, NULL);
    glCompileShader(shader_type);

    free(f_content);
    return shader_type;
}

void deleteShader(unsigned int shader)
{
    glDeleteShader(shader);
}

