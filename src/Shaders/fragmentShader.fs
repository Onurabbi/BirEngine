#version 330 core
out vec4 FragColor;
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

struct Data{
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
};

in DirLight dirLightOut;
in PointLight pointLightsOut[NR_OF_POINT_LIGHTS];
in SpotLight spotLightOut;

in VS_OUT{
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
}fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

uniform float shininess;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, Data data);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, Data data);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, Data data);

void main()
{
	Data data;

	data.FragPos = fs_in.FragPos;
	data.TangentFragPos = fs_in.TangentFragPos;
	data.TangentViewPos = fs_in.TangentViewPos;
	data.TexCoords = fs_in.TexCoords;

	vec3 normal = vec3(texture(texture_normal1, data.TexCoords));
	normal = normalize(normal*2.0-1.0);
	vec3 viewDir = normalize(data.TangentViewPos-data.TangentFragPos);
	vec3 result = calcDirLight(dirLightOut, normal, viewDir, data);
	for(int i=0; i<NR_OF_POINT_LIGHTS; i++)
		result += calcPointLight(pointLightsOut[i], normal, viewDir, data);
	result+=calcSpotLight(spotLightOut, normal, viewDir, data);
	result = pow(result, vec3(1.0/2.2));
	FragColor = vec4(result, 1.0);
}	

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, Data data)
{
	vec3 lightDir = normalize(-light.direction);
	//diffuse shading
	float diff = max(dot(lightDir, normal), 0.0);
	//specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir+viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	//combine results
	vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, data.TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, data.TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(texture_specular1, data.TexCoords));
	return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, Data data)
{
	vec3 lightDir = normalize(light.position-data.TangentFragPos);

	//diffuse shading
	float diff = max(dot(lightDir, normal), 0.0);
	//specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir+viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	//attenuation
	float dist = length(light.position-data.TangentFragPos);
	float attenuation = 1.0 / dist;
	//combine results
	vec3 ambient = light.ambient*vec3(texture(texture_diffuse1, data.TexCoords));
	vec3 diffuse = light.diffuse*diff*vec3(texture(texture_diffuse1, data.TexCoords));
	vec3 specular = light.specular*spec*vec3(texture(texture_specular1, data.TexCoords));
	ambient*=attenuation;
	diffuse*=attenuation;
	specular*=attenuation;
	return (ambient+diffuse+specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, Data data)
{
	vec3 lightDir = normalize(light.position-data.TangentFragPos);
	//diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	//specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir+viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	//attenuation
	float dist = length(light.position-data.TangentFragPos);
	float attenuation = 1.0 / dist;
	//spotlight intensity
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff-light.outerCutOff;
	float intensity = clamp((theta-light.outerCutOff)/epsilon, 0.0, 1.0);
	//combine results
	vec3 ambient = light.ambient*vec3(texture(texture_diffuse1, data.TexCoords));
	vec3 diffuse = light.diffuse*diff*vec3 (texture(texture_diffuse1, data.TexCoords));
	vec3 specular = light.specular*spec*vec3(texture(texture_specular1, data.TexCoords));
	ambient*=attenuation*intensity;
	diffuse*=attenuation*intensity;
	specular*=attenuation*intensity;
	return(ambient+diffuse+specular);
}
