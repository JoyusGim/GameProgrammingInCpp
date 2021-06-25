#version 330

layout(location=0) in vec3 inPosition;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec2 inTexCoord;

uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragWorldPos;

void main()
{
	// 아직 모델스페이스상의 좌표이다.
	vec4 pos = vec4(inPosition, 1.0);
	pos = pos * uWorldTransform;
	fragWorldPos = pos.xyz;
	gl_Position = pos * uViewProj;

	fragNormal = (vec4(inNormal, 0.0) * uWorldTransform).xyz;

	fragTexCoord = inTexCoord;
}