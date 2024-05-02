#include "mesh.h"

#include <glad/glad.h>

Mesh setupMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
    Mesh mesh;
    //-----------------------------------------------------------------------------
    // Create buffers/arrays
    //-----------------------------------------------------------------------------
    // Generate and bind the Vertex Array Object
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Generate and bind the Vertex Buffer Object
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // Generate and bind the Element Buffer Object
    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    //-----------------------------------------------------------------------------
    // Set vertex attributes pointers 
    //-----------------------------------------------------------------------------
    // Position 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind VAO
    glBindVertexArray(0);

    mesh.vertices = vertices;
    mesh.indices = indices;

    return mesh;
}