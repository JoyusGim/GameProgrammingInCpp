#include "MeshComponent.h"
#include "Mesh.h"
#include "Shader.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"

MeshComponent::MeshComponent(Actor* owner, bool isSkeletal /* = false */)	:
	Component(owner),
	mMesh{ nullptr },
	mTextureIndex{ 0 },
	mIsSkeletal{ isSkeletal }
{
	mOwner->GetGame()->GetRenderer()->AddMesh(this);
}

MeshComponent::~MeshComponent()
{
	mOwner->GetGame()->GetRenderer()->RemoveMesh(this);
}

void MeshComponent::Draw(Shader* shader)
{
	if (mMesh)
	{
		shader->SetMatrixUniform("uWorldTransform",
			mOwner->GetWorldTransform());
		shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());

		Texture* t = mMesh->GetTexture(mTextureIndex);
		if (t) t->SetActive();
		VertexArray* va = mMesh->GetVertexArray();
		va->SetActive();
		glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
	}
}

void MeshComponent::SetMesh(Mesh* mesh)
{
	mMesh = mesh;
}

void MeshComponent::SetTextureIndex(size_t index)
{
	mTextureIndex = index;
}

bool MeshComponent::GetIsSkeletal() const
{
	return mIsSkeletal;
}
