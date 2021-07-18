#pragma once
#include "Component.h"

class MeshComponent	: public Component
{
protected:
	class Mesh* mMesh;
	size_t mTextureIndex;
	bool mIsSkeletal;

public:
	MeshComponent(class Actor* owner, bool isSkeletal = false);
	~MeshComponent();

	virtual void Draw(class Shader* shader);
	virtual void SetMesh(class Mesh* mesh);
	void SetTextureIndex(size_t index);

	bool GetIsSkeletal() const;
};

