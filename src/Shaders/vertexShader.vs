#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;

#define NR_OF_POINT_LIGHTS 2

struct DirLight
{
	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform DirLight dirLight;
uniform PointLight pointLights[NR_OF_POINT_LIGHTS];
uniform SpotLight spotLight;

out DirLight dirLightOut;
out PointLight pointLightsOut[NR_OF_POINT_LIGHTS];
out SpotLight spotLightOut;

out VS_OUT{
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
}vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 viewPos;

void main()
{
	//Fragpos, texcoords and viewpos in TBN
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.TexCoords = aTexCoords;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix*aTangent);
	vec3 N = normalize(normalMatrix*aNormal);
	T = normalize(T-dot(T, N)*N);
	vec3 B = cross(N, T);

	mat3 TBN = transpose(mat3(T, B, N));
	vs_out.TangentViewPos = TBN*viewPos;
	vs_out.TangentFragPos = TBN*vs_out.FragPos;

	//directional light;
	dirLightOut.direction = TBN*dirLight.direction;
	dirLightOut.position = TBN*dirLight.position;
	dirLightOut.ambient = dirLight.ambient;
	dirLightOut.diffuse = dirLight.diffuse;
	dirLightOut.specular = dirLight.specular;
	//point lights
	for(int i=0; i<NR_OF_POINT_LIGHTS; i++)
	{
		pointLightsOut[i].position = TBN*pointLights[i].position;
		pointLightsOut[i].constant = pointLights[i].constant;
		pointLightsOut[i].linear = pointLights[i].linear;
		pointLightsOut[i].quadratic = pointLights[i].quadratic;
		pointLightsOut[i].ambient = pointLights[i].ambient;
		pointLightsOut[i].diffuse = pointLights[i].diffuse;
		pointLightsOut[i].specular = pointLights[i].specular;
	}
	//spot light
	spotLightOut.position = TBN*spotLight.position;
	spotLightOut.direction = TBN*spotLight.direction;
	spotLightOut.constant = spotLight.constant;
	spotLightOut.linear = spotLight.linear;
	spotLightOut.quadratic = spotLight.quadratic;
	spotLightOut.ambient = spotLight.ambient;
	spotLightOut.diffuse = spotLight.diffuse;
	spotLightOut.specular = spotLight.specular;
	spotLightOut.cutOff = spotLight.cutOff;
	spotLightOut.outerCutOff = spotLight.outerCutOff;

	gl_Position = projection*view*model*vec4(aPos, 1.0);
}