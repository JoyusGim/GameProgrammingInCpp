#include "VertexArray.h"
#include <glew.h>

VertexArray::VertexArray(const float verts[], unsigned int numVerts, const unsigned int indices[], unsigned int numIndices) :
    mNumVerts{ numVerts }, mNumIndices{ numIndices },
    mVertexBuffer{ 0 }, mIndexBuffer{ 0 }, mVertexArray{ 0 }
{
    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);

    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        numVerts * 8 * sizeof(float),
        verts,
        GL_STATIC_DRAW
    );

    glGenBuffers(1, &mIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        numIndices * sizeof(unsigned int),
        indices,
        GL_STATIC_DRAW
    );

    // 위치
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float) * 8,
        0
    );

    // 노말
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float) * 8,
        reinterpret_cast<void*>(sizeof(float) * 3)
    );

    // 텍셀
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float) * 8,
        reinterpret_cast<void*>(sizeof(float) * 6)
    );
}

VertexArray::~VertexArray()
{
    glDeleteBuffers(1, &mVertexBuffer);
    glDeleteBuffers(1, &mIndexBuffer);
    glDeleteVertexArrays(1, &mVertexArray);
}

void VertexArray::SetActive()
{
    glBindVertexArray(mVertexArray);
}

unsigned int VertexArray::GetNumIndices() const
{
    return mNumIndices;
}

unsigned int VertexArray::GetNumVerts() const
{
    return mNumVerts;
}
