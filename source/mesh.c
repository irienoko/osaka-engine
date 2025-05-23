#include "backbone/mesh.h"


#include <glad/glad.h>
#include <thirdparty/vec.h>

#define STB_IMAGE_IMPLEMENTATION
#include <thirdparty/stb_image.h>


Mesh *createPackedMesh(Mesh_index m_i)
{
    Mesh *mesh_packed = vector_create();

    for(int i = 0; i < m_i.nverts; i++)
    {
        Mesh m = {0};
        m.position = m_i.out_verts[i];
        m.normal = m_i.out_norms[i];
        m.texCoord = m_i.out_uvs[i];

        m.indices_count = (int)m_i.indices_num;
        m.indices = m_i.indices;

        vector_add(&mesh_packed, m);
    }



    return mesh_packed;
}

opengl_data setUpMesh(Mesh *m_p, const char *texture_path)
{
    opengl_data obj = {0};

    glGenBuffers(1, &obj.VBO);
    glGenVertexArrays(1, &obj.VAO);
    glGenBuffers(1, &obj.EBO);

    glBindVertexArray(obj.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO);

    //load int all mesh data
    glBufferData(GL_ARRAY_BUFFER, vector_size(m_p) * sizeof(Mesh), &m_p[0], GL_STATIC_DRAW);

    //position vertex
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Mesh), (void*)offsetof(Mesh, position));

    //texture vertex
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Mesh), (void*)offsetof(Mesh, texCoord));

    //normal vertex

    //bind element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_p->indices_count * sizeof(GL_UNSIGNED_INT),& m_p->indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    glGenTextures(1, &obj.texture);
    glBindTexture(GL_TEXTURE_2D, obj.texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(texture_path, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    return obj;
}

void drawMesh(Mesh *m_p, opengl_data obj)
{
    glBindVertexArray(obj.VAO);
    glDrawElements(GL_TRIANGLES, m_p->indices_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}