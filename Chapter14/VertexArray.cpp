#include "VertexArray.h"
#include <glew.h>

VertexArray::VertexArray(const void* verts, unsigned int numVerts, Layout layout, const unsigned int indices[], unsigned int numIndices) :
    mNumVerts{ numVerts }, mNumIndices{ numIndices },
    mVertexBuffer{ 0 }, mIndexBuffer{ 0 }, mVertexArray{ 0 }
{
    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);

    unsigned vertexSize = 8 * sizeof(float);
    if (layout == Layout::PosNormSkinTex)
    {
        vertexSize = 8 * sizeof(float) + 8 * sizeof(char);
    }

    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, numVerts * vertexSize, verts, GL_STATIC_DRAW);

    glGenBuffers(1, &mIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    if (layout == PosNormTex)
    {
        // 위치
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, 0);

        // 노말
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize,
            reinterpret_cast<void*>(sizeof(float) * 3));

        // 텍셀
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertexSize, 
            reinterpret_cast<void*>(sizeof(float) * 6));
    }
    else if (layout == PosNormSkinTex)
    {
        // 위치
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, 0);

        // 노말
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexSize,
            reinterpret_cast<void*>(sizeof(float) * 3));

        // 스키닝 본
        glEnableVertexAttribArray(2);
        glVertexAttribIPointer(2, 4, GL_UNSIGNED_BYTE, vertexSize,
            reinterpret_cast<void*>(sizeof(float) * 6));

        // 스키닝 본 가중치
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, vertexSize,
            reinterpret_cast<void*>(sizeof(float) * 6 + sizeof(char) * 4));

        // 텍셀
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, vertexSize,
            reinterpret_cast<void*>(sizeof(float) * 6 + sizeof(char) * 8));
    }
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

unsigned int VertexArray::GetVertexSize(VertexArray::Layout layout)
{
    unsigned vertexSize = 8 * sizeof(float);
    if (layout == PosNormSkinTex)
    {
        vertexSize = 8 * sizeof(float) + 8 * sizeof(char);
    }
    return vertexSize;
}
