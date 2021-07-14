#include "PointLightComponent.h"
#include "Actor.h"
#include "Mesh.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Game.h"
#include "Renderer.h"

PointLightComponent::PointLightComponent(Actor* owner)	:
	Component(owner)
{
	owner->GetGame()->GetRenderer()->AddPointLight(this);
}

PointLightComponent::~PointLightComponent()
{
	mOwner->GetGame()->GetRenderer()->RemovePointLight(this);
}

void PointLightComponent::Draw(Shader* shader, Mesh* mesh)
{
	Matrix4 scale = Matrix4::CreateScale(mOwner->GetScale() * mOuterRadius / mesh->GetRadius());
	Matrix4 trans = Matrix4::CreateTranslation(mOwner->GetPosition());
	Matrix4 worldTransform = scale * trans;
	shader->SetMatrixUniform("uWorldTransform", worldTransform);

	shader->SetVectorUniform("uPointLight.mWorldPos", mOwner->GetPosition());
	shader->SetVectorUniform("uPointLight.mDiffuseColor", mDiffuseColor);
	shader->SetFloatUniform("uPointLight.mInnerRadius", mInnerRadius);
	shader->SetFloatUniform("uPointLight.mOuterRadius", mOuterRadius);

	glDrawElements(GL_TRIANGLES, mesh->GetVertexArray()->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
}

void PointLightComponent::SetDiffuseColor(const Vector3& color)
{
	mDiffuseColor = color;
}

void PointLightComponent::SetInnerRadius(float radius)
{
	mInnerRadius = radius;
}

void PointLightComponent::SetOuterRadius(float radius)
{
	mOuterRadius = radius;
}
