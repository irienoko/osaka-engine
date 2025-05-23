#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "backbone/shader.h"
#include "backbone/objparser.h"
#include "backbone/mesh.h"

#include "linmath.h"
#include "thirdparty/vec.h"

void error_callback(int error, const char* description);

int main(int argc, char **argv)
{
    glfwSetErrorCallback(error_callback);
    if(!glfwInit())
    {
        printf("failed to initialise glfw :( quitting");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(640, 480, "hi again", NULL, NULL);
    if(!window)
    {
        printf("failed to create window :( quitting");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize OpenGL context\n");
        exit(EXIT_FAILURE);
    }
    glfwSwapInterval(1);
    

    //load basic shader
    unsigned int vertexShader = loadShader("bin/assets/vertex.vs", GL_VERTEX_SHADER);
    unsigned int fragmentShader = loadShader("bin/assets/fragment.fs", GL_FRAGMENT_SHADER);

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    deleteShader(vertexShader);
    deleteShader(fragmentShader);

    //end of load basic shader


    Model orange = loadObjFromFile("bin/models/teto_orange.obj");
    Mesh_index orange_mesh = indexVBO(orange);
    Mesh *orange_packed_mesh = createPackedMesh(orange_mesh);
    opengl_data orange_obj = setUpMesh(orange_packed_mesh, "bin/images/teto_orange.jpg");

    while(!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.2f, 0.3f,0.3f,1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        mat4x4 view;
        mat4x4_identity(view);
        mat4x4_translate(view, 0.0f, 0.0f, -4.0f);

        mat4x4 projection;
        mat4x4_identity(projection);
        mat4x4_perspective(projection, 45.0f, width / height, 0.1f, 100.0f);

    
        unsigned int ViewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, (float*)view);

        unsigned int ProjectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(ProjectionLoc, 1, GL_FALSE, (float*)projection);

        glBindTexture(GL_TEXTURE_2D, orange_obj.texture);
        mat4x4 model1;
        mat4x4_identity(model1);
        mat4x4_translate(model1, 1.0f, 0.0f, 0.0f); 
        mat4x4_rotate(model1, model1, 0.3f, 1.0f, 0.0f, (float)glfwGetTime() * 1.0f);
        mat4x4_scale_aniso(model1, model1, 0.4f, 0.4f, 0.4f);

        unsigned int ModelLoc1 = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(ModelLoc1, 1, GL_FALSE, (float*)model1);
        drawMesh(orange_packed_mesh, orange_obj);




        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void error_callback(int error, const char* description)
{
    fprintf(stderr, ":( Error: %s\n", description);
}