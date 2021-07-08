#include "SkeletalMeshComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "Shader.h"

SkeletalMeshComponent::SkeletalMeshComponent(Actor* owner)	:
	MeshComponent(owner, true)
{
}

void SkeletalMeshComponent::Draw(Shader* shader)
{
	MeshComponent::Draw(shader);
}
