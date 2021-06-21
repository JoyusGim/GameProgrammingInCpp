#version 330

uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

in vec3 inPosition;

void main()
{
	// ���� �𵨽����̽����� ��ǥ�̴�.
	vec4 pos = vec4(inPosition, 1.0);

	gl_Position = pos * uWorldTransform * uViewProj;
}