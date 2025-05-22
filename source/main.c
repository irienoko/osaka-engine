#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "backbone/shader.h"
#include "backbone/objparser.h"

#include "linmath.h"
#include "thirdparty/vec.h"

#define STB_IMAGE_IMPLEMENTATION
#include <thirdparty/stb_image.h>

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
    Model m = loadObjFromFile("bin/models/cube.obj");
    Mesh mesh = createMeshFromModel(m);

    unsigned int VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    float *packed_vertex = vector_create();
    float *packed_uv = vector_create();
    float *packed_normal = vector_create();

    for(int i = 0; i < mesh.nverts; i++)
    {
        for (int j =0; j < 3; j++)
        {
            vector_add(&packed_vertex,  mesh.out_verts[i].v[j]);
            vector_add(&packed_uv,  mesh.out_verts[i].t[j]);
            vector_add(&packed_normal,  mesh.out_norms[i].v[j]);
        }
    }

    unsigned vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    
    glBufferData(GL_ARRAY_BUFFER, vector_size(packed_vertex) * sizeof(float), &packed_vertex[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    unsigned uv_buffer;
    glGenBuffers(1, &uv_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);

    glBufferData(GL_ARRAY_BUFFER, vector_size(packed_uv) * sizeof(float), &packed_vertex[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("bin/images/wall.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    /*
    unsigned norm_buffer;
    glGenBuffers(1, &norm_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, norm_buffer);

    glBufferData(GL_ARRAY_BUFFER, vector_size(packed_normal) * sizeof(float), &packed_normal[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);*/


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices_num * sizeof(GL_UNSIGNED_INT),& mesh.indices[0], GL_STATIC_DRAW);



    //glBindVertexArray(0);

    printf("vertex count %lld\n", m.nverts);
    printf("norm count %lld\n", m.nnorms);
    printf("uv count %lld\n", m.nuvs);


    while(!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.2f, 0.3f,0.3f,1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
        
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        mat4x4 model;
        mat4x4_identity(model);
        mat4x4_translate(model, -1.0f, 0.0f, 0.0f); 
        mat4x4_rotate(model, model, 0.3f, 1.0f, 0.0f, (float)glfwGetTime() * 1.0f);
        mat4x4_scale_aniso(model, model, 0.4f, 0.4f, 0.4f);

        unsigned int ModelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, (float*)model);

        glDrawElements(GL_TRIANGLES, mesh.indices_num, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        glUseProgram(shaderProgram);

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