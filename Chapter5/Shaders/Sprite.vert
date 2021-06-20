#version 330

layout(location=0) in vec3 inPosition;
layout(location=1) in vec2 inTexCoord;

uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

out vec2 fragTexCoord;

void main()
{
	// 아직 모델스페이스상의 좌표이다.
	vec4 pos = vec4(inPosition, 1.0);

	gl_Position = pos * uWorldTransform * uViewProj;

	fragTexCoord = inTexCoord;
}