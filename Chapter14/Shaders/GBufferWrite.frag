#version 330

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragWorldPos;

layout(location = 0) out vec3 outDiffuse;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outWorldPos;
layout(location = 3) out vec3 outSpecPower;

uniform sampler2D uTexture;
uniform float uSpecPower;

void main()
{
	outDiffuse = texture(uTexture, fragTexCoord).xyz;
	outNormal = fragNormal;
	outWorldPos = fragWorldPos;
	outSpecPower = vec3(uSpecPower, 0.0, 0.0);
}