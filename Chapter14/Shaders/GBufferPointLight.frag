#version 330

in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

uniform sampler2D uGDiffuse;
uniform sampler2D uGNormal;
uniform sampler2D uGWorldPos;
uniform sampler2D uGSpecPower;

struct PointLight
{
	vec3 mWorldPos;
	vec3 mDiffuseColor;
	vec3 mSpecColor;
	float mInnerRadius;
	float mOuterRadius;
};

uniform vec3 uCameraPos;
uniform PointLight uPointLight;
uniform vec2 uScreenDimensions;

void main()
{
	vec2 gbufferCoord = gl_FragCoord.xy / uScreenDimensions;

	vec3 gbufferDiffuse = texture(uGDiffuse, gbufferCoord).xyz;
	vec3 gbufferNorm = texture(uGNormal, gbufferCoord).xyz;
	vec3 gbufferWorldPos = texture(uGWorldPos, gbufferCoord).xyz;
	float gbufferSpecPower = texture(uGSpecPower, gbufferCoord).x;

	vec3 N = normalize(gbufferNorm);
	vec3 L = normalize(uPointLight.mWorldPos - gbufferWorldPos);
	vec3 V = normalize(uCameraPos - gbufferWorldPos);
	vec3 R = normalize(reflect(-L, N));
	
	vec3 Phong = vec3(0.0, 0.0, 0.0);
	float NdotL = dot(N, L);
	if(NdotL > 0)
	{
		float dist = distance(uPointLight.mWorldPos, gbufferWorldPos);
		float intensity = smoothstep(uPointLight.mInnerRadius, uPointLight.mOuterRadius, dist);
		vec3 DiffuseColor = mix(uPointLight.mDiffuseColor, vec3(0.0, 0.0, 0.0), intensity);		
		vec3 Specular = uPointLight.mSpecColor * pow(max(0.0, dot(R, V)), gbufferSpecPower);

		Phong = DiffuseColor * NdotL;
		Phong += Specular;
	}
	outColor = vec4(gbufferDiffuse * Phong, 1.0);
}