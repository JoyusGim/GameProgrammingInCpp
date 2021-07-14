#version 330

in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

uniform sampler2D uGDiffuse;
uniform sampler2D uGNormal;
uniform sampler2D uGWorldPos;

struct DirectionalLight
{
	// Direction of light
	vec3 mDirection;
	// Diffuse color
	vec3 mDiffuseColor;
	// Specular color
	vec3 mSpecColor;
};

uniform vec3 uCameraPos;
uniform float uSpecPower;
uniform vec3 uAmbientLight;
uniform DirectionalLight uDirLight;

void main()
{
	vec3 gbufferDiffuse = texture(uGDiffuse, fragTexCoord).xyz;
	vec3 gbufferNorm = texture(uGNormal, fragTexCoord).xyz;
	vec3 gbufferWorldPos = texture(uGWorldPos, fragTexCoord).xyz;

	vec3 N = normalize(gbufferNorm);
	vec3 L = normalize(-uDirLight.mDirection);
	vec3 V = normalize(uCameraPos - gbufferWorldPos);
	vec3 R = normalize(reflect(-L, N));

	vec3 Phong = uAmbientLight;
	float NdotL = dot(N, L);
	if(NdotL > 0)
	{
		vec3 Diffuse = uDirLight.mDiffuseColor * NdotL;
		Phong += Diffuse;
	}

	outColor = vec4(gbufferDiffuse * Phong, 1.0);
}