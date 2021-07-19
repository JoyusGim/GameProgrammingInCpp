#pragma once
class VertexArray
{
public:
	enum Layout { PosNormTex, PosNormSkinTex };

private:
	unsigned int mNumVerts;
	unsigned int mNumIndices;
	unsigned int mVertexBuffer;
	unsigned int mIndexBuffer;
	unsigned int mVertexArray;

public:
	VertexArray(const void* verts, unsigned int numVerts, Layout layout, const unsigned int indices[], unsigned int numIndices);
	~VertexArray();

	void SetActive();
	
	unsigned int GetNumIndices() const;
	unsigned int GetNumVerts() const;

	static unsigned int GetVertexSize(VertexArray::Layout layout);
};

