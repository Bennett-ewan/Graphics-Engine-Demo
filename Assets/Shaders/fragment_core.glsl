#version 330 core

#define MAX_LIGHTS 12
#define PI 3.1415926535897932384626433832795

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	float reflectiveness;
	float refraction_index;

	sampler2D texture_diffuse1;
	sampler2D texture_diffuse2;
	sampler2D texture_specular1;
	sampler2D texture_specular2;
	sampler2D texture_normal1;
};

struct PointLight
{
	vec3 pos;
	float intensity;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

    float constant;
	float linear;
	float quadratic;

	mat4 spaceMatrix;
};

struct DirectionalLight
{
	vec3 direction;
	float intensity;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in VS_OUT {
	vec3 position;
	vec2 texcoord;
	vec3 normal;
	mat3 TBN;
	vec4 posLightSpace;
} fs_in;

out vec4 fs_color;

uniform int has_texture;
uniform int has_specular;
uniform int has_diffuse;
uniform int has_normal;
uniform vec2 texture_scale;
uniform float opacity;

uniform Material material;
uniform vec3 camera_pos;

uniform int num_point_lights, num_dir_lights;
uniform PointLight pointlights[MAX_LIGHTS];
uniform DirectionalLight dirlight[MAX_LIGHTS];

uniform samplerCube skybox;
uniform sampler2D shadowMap;

// Lighting calculation
vec2 uv;
vec3 Kd;
vec3 N;
vec3 V;

// Prototypes
vec3 calc_point_light(PointLight light);
vec3 calc_dir_light(DirectionalLight light);
vec3 calc_point_light_phong(PointLight light);
vec3 calc_dir_light_phong(DirectionalLight light);
float shadow_calculation(vec4 posLightSpace, vec3 L);


void main()
{
	float alpha = 1.0;

	uv = fs_in.texcoord * texture_scale;
	N = normalize(fs_in.normal);
	V = normalize(camera_pos - fs_in.position);

	float ratio = 1.0 / material.refraction_index;
	vec3 I = normalize(fs_in.position - camera_pos);
	vec3 R;
	if (ratio == 1)
		R = reflect(I, normalize(N));
	else
		R = refract(I, normalize(N), ratio);

	if (has_texture == 1)
	{
		vec4 tex = mix(texture(material.texture_diffuse1, uv), vec4(texture(skybox, R).rbg, 1.0), material.reflectiveness);
		alpha = tex.a;
		Kd = tex.rgb;
	}
	else
		Kd = mix(vec4(material.diffuse, 1.f), vec4(texture(skybox, R).rgb, 1.0), material.reflectiveness).rgb;


	if (has_normal == 1)
	{
		N = texture(material.texture_normal1, uv).rgb;
		N = normalize(N * 2.0 - 1.0);
		N = normalize(fs_in.TBN * N);
	}
	
	vec3 result;
	for (int i = 0; i < num_dir_lights; i++)
	{
		result += calc_dir_light(dirlight[i]);
		//result += calc_dir_light_phong(dirlight[i]);
	}
	for (int i = 0; i < num_point_lights; i++)
	{
		result += calc_point_light(pointlights[i]);
		//result += calc_point_light_phong(pointlights[i]);
	}
		 
	fs_color = vec4(result, alpha * opacity);

}





vec3 calc_point_light(PointLight light)
{
	vec3 L = normalize(light.pos - fs_in.position);
	vec3 H = normalize(L + V);
	float LN = max(dot(L, N), 0.0);
	float NH = max(dot(N, H), 0.0);
	float LH = max(dot(L, H), 0.0);

	float distance = length(light.pos - fs_in.position);
	//float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	float attenuation = (1.0 / distance) * 10;

	//vec4 FragPosLightSpace = light.spaceMatrix * vec4(fs_in.position, 1.0);
	//float shadow = shadow_calculation(fs_in.posLightSpace, L);

	//vec3 Ks = material.specular * light.specular * attenuation;
	vec3 Ks = material.specular * light.specular;
	if (has_specular == 1)
		Ks = vec3(texture(material.texture_specular1, uv));

	float D = ((material.shininess + 2) / (2 * PI)) * pow(NH, material.shininess);
	vec3 F = Ks + ((vec3(1, 1, 1) - Ks) * pow(1 - LH, 5));
	vec3 BRDF = (Kd / PI) + ((D * F) / (4 * pow(LH, 2)));
	return  light.intensity * (((light.ambient * attenuation) * (material.ambient * Kd)) + (light.diffuse * attenuation * LN * BRDF));
	//return  (((light.ambient * attenuation) * (material.ambient + Kd + (1.0 - shadow))) + (light.diffuse * attenuation * LN * BRDF));
	//return  light.intensity * (((light.ambient) * (material.ambient + Kd)) + (light.diffuse * LN * BRDF));
}

vec3 calc_dir_light(DirectionalLight light)
{
	vec3 L = normalize(-light.direction);
	vec3 H = normalize(L + V);
	float LN = max(dot(L, N), 0.0);
	float NH = max(dot(N, H), 0.0);
	float LH = max(dot(L, H), 0.0);

	vec3 Ks = material.specular * light.specular;
	if (has_specular == 1)
		Ks = vec3(texture(material.texture_specular1, uv));

	float D = ((material.shininess + 2) / (2 * PI)) * pow(NH, material.shininess);
	vec3 F = Ks + ((vec3(1, 1, 1) - Ks) * pow(1 - LH, 5));
	vec3 BRDF = (Kd / PI) + ((D * F) / (4 * pow(LH, 2)));
	return light.intensity * ((light.ambient * (material.ambient * Kd)) + (light.diffuse * LN * BRDF));
}

float shadow_calculation(vec4 posLightSpace, vec3 L)
{
	vec3 projCoords = posLightSpace.xyz / posLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	vec3 normal = normalize(fs_in.normal);
	float bias = max(0.05 * (1.0 - dot(normal, L)), 0.005);
	
	float shadow = 0.0;
	vec2 texel_size = 1.0 / textureSize(shadowMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texel_size).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;
	if (projCoords.z > 1.0)
		shadow = 0.0;
	return shadow;
}


vec3 calc_point_light_phong(PointLight light)
{
	vec3 L = normalize(light.pos - fs_in.position);
	vec3 ambient = light.intensity * light.ambient * Kd * material.ambient;
	float diff = max(dot(L, N), 0.0);
	vec3 diffuse = diff * light.intensity * light.diffuse * material.diffuse;
	vec3 R = reflect(-L, N);
	vec3 H = normalize(L + V);
	float spec = pow(max(dot(N, H), 0.0), material.shininess);
	if (material.shininess == 0.0)
		spec = 0.0;
	vec3 specular = light.specular * light.intensity * spec * material.specular;
	return (ambient + diffuse + specular);
}

vec3 calc_dir_light_phong(DirectionalLight light)
{
	vec3 L = normalize(-light.direction);
	vec3 ambient = light.intensity * light.ambient * Kd;
	float diff = max(dot(L, N), 0.0);
	vec3 diffuse = diff * light.intensity * light.diffuse;
	vec3 R = reflect(-L, N);
	vec3 H = normalize(L + V);
	float spec = pow(max(dot(N, H), 0.0), 32.0);
	vec3 specular = light.specular * light.intensity * spec;
	return (ambient + diffuse + specular);
}