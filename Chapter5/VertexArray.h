#pragma once
class VertexArray
{
	unsigned int mNumVerts;
	unsigned int mNumIndices;
	unsigned int mVertexBuffer;
	unsigned int mIndexBuffer;
	unsigned int mVertexArray;

public:
	VertexArray(const float verts[], unsigned int numVerts, const unsigned int indices[], unsigned int numIndices);
	~VertexArray();

	void SetActive();
	
	unsigned int GetNumIndices() const;
	unsigned int GetNumVerts() const;
};

