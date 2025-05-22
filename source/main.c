#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "backbone/shader.h"
#include "backbone/objparser.h"

#include "linmath.h"

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
    


    float vertices[]=
    {
        1.000000, -1.000000, -1.000000,
        1.000000, -1.000000, 1.000000,
       -1.000000, -1.000000, 1.000000,
       -1.000000, -1.000000, -1.000000,
       1.000000, 1.000000, -1.000000,
       0.999999, 1.000000, 1.000001,
       -1.000000, 1.000000, 1.000000,
       -1.000000, 1.000000, -1.000000,
    };

    unsigned int indices[] =
    {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };  

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
    Model m = loadObjFromFile("bin/models/osaka.obj");
    Model m1 = loadObjFromFile("bin/models/monkey.obj");

    unsigned int VBO[2], VAO[2], EBO[2];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(2, EBO);


    glBindVertexArray(VAO[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);


    glBufferData(GL_ARRAY_BUFFER, m.nverts* sizeof(float), &m.verts[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.nfaces * sizeof(GL_UNSIGNED_INT), &m.pos_indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);

    glBindVertexArray(VAO[1]);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);


    glBufferData(GL_ARRAY_BUFFER, m1.nverts* sizeof(float), &m1.verts[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m1.nfaces * sizeof(GL_UNSIGNED_INT), &m1.pos_indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);
    



    while(!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.2f, 0.3f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

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

        glUseProgram(shaderProgram);

        glBindVertexArray(VAO[0]);
        mat4x4 model;
        mat4x4_identity(model);
        mat4x4_translate(model, -1.0f, 0.0f, 0.0f); 
        mat4x4_rotate(model, model, 0.3f, 1.0f, 0.0f, (float)glfwGetTime() * 1.0f);
        mat4x4_scale_aniso(model, model, 0.4f, 0.4f, 0.4f);

        unsigned int ModelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, (float*)model);

        glDrawElements(GL_TRIANGLES, m.nfaces, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glBindVertexArray(VAO[1]);
        mat4x4 model1;
        mat4x4_identity(model1);
        mat4x4_rotate(model1, model1, 0.3f, 1.0f, 0.0f, (float)glfwGetTime() * 2.0f);
        mat4x4_scale_aniso(model1, model1, 0.4f, 0.4f, 0.4f);

        unsigned int ModelLoc1 = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(ModelLoc1, 1, GL_FALSE, (float*)model1);
        glDrawElements(GL_TRIANGLES, m1.nfaces, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


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