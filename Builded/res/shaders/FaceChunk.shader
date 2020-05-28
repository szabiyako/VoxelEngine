#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in float texturepos;
layout(location = 2) in float normal;

//Base
out vec2 texturePos;
out vec3 Normal;

out float textureType;

//light out
out vec3 fragPos;
out float range;

uniform mat4 u_ViewProj;
uniform mat4 u_Model;
uniform vec4 u_posLight;

uniform float u_WaterWave;

float rand(vec2 co)
{
	return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 GetOffset()
{
	//return floor(position);
	//return vec3(3.f, 3.f, 3.f);
	//return vec3(floor(position.x), floor(position.y), floor(position.z));
	//return floor(vec3(0.7f, 0.7f, 0.7f));
	return vec3(0.f, 0.f, 0.f);
}

vec3 GetNormal()
{
	if (normal < 0.5f)//FRONT
		return vec3(0.f, 0.f, 1.f);
	else if (normal < 1.5f)//LEFT
		return vec3(-1.f, 0.f, 0.f);
	else if (normal < 2.5f)//TOP
		return vec3(0.f, 1.f, 0.f);
	else if (normal < 3.5f)//BOTTOM
		return vec3(0.f, -1.f, 0.f);
	else if (normal < 4.5f)//RIGHT
		return vec3(1.f, 0.f, 0.f);
	else if (normal < 5.5f)//BACK
		return vec3(0.f, 0.f, -1.f);
}

vec2 GetTexturePos()
{
	//if (normal < 0.5f)//FRONT
	//	return position.xy - offset.xy;
	//else if (normal < 1.5f)//LEFT
	//	return position.zy - offset.zy;
	//else if (normal < 2.5f)//TOP
	//	return vec2(position.x - offset.x, 1.f - (position.z - offset.z));
	//else if (normal < 3.5f)//BOTTOM
	//	return position.xz - offset.xz;
	//else if (normal < 4.5f)//RIGHT
	//	return vec2(1.f - (position.z - offset.z), position.y - offset.y);
	//else if (normal < 5.5f)//BACK
	//	return vec2(1.f - (position.x - offset.x), position.y - offset.y);

	//using texturepos to change textures
	textureType = 0.f; // grass
	if (texturepos < 0.5f)
		return vec2(0.f, 1.f);
	else if (texturepos < 1.5f)
		return vec2(1.f, 1.f);
	else if (texturepos < 2.5f)
		return vec2(0.f, 0.f);
	else if (texturepos < 3.5f)
		return vec2(1.f, 0.f);

	textureType = 1.f; //water
	if (texturepos < 4.5f)
		return vec2(0.f, 1.f);
	else if (texturepos < 5.5f)
		return vec2(1.f, 1.f);
	else if (texturepos < 6.5f)
		return vec2(0.f, 0.f);
	else if (texturepos < 7.5f)
		return vec2(1.f, 0.f);

	textureType = 2.f; //sand
	if (texturepos < 8.5f)
		return vec2(0.f, 1.f);
	else if (texturepos < 9.5f)
		return vec2(1.f, 1.f);
	else if (texturepos < 10.5f)
		return vec2(0.f, 0.f);
	else if (texturepos < 11.5f)
		return vec2(1.f, 0.f);

	textureType = 3.f; //dirt
	if (texturepos < 12.5f)
		return vec2(0.f, 1.f);
	else if (texturepos < 13.5f)
		return vec2(1.f, 1.f);
	else if (texturepos < 14.5f)
		return vec2(0.f, 0.f);
	else if (texturepos < 15.5f)
		return vec2(1.f, 0.f);

	textureType = 4.f; //snow
	if (texturepos < 16.5f)
		return vec2(0.f, 1.f);
	else if (texturepos < 17.5f)
		return vec2(1.f, 1.f);
	else if (texturepos < 18.5f)
		return vec2(0.f, 0.f);
	else if (texturepos < 19.5f)
		return vec2(1.f, 0.f);
}

void main()
{
	vec4 worldCoords = u_Model * vec4(position, 1.0);
	gl_Position = u_ViewProj * worldCoords;

	range = distance(worldCoords, u_posLight);
	fragPos = worldCoords.xyz;

	//temp water
	if (texturepos > 3.5f && texturepos < 7.5)
	{
		//gl_Position = u_ViewProj * (worldCoords + vec4(0, sin(worldCoords.x + worldCoords.z), 0, 0));
	
		//worldCoords = worldCoords + vec4(0, (sin(worldCoords.x) + sin(worldCoords.z)) * 0.1 - 0.3, 0, 0);
		//float tmp = (sin(worldCoords.x + u_WaterWave) + sin(worldCoords.z + u_WaterWave)) * 0.1 - 0.3 + rand(vec2(worldCoords.x, worldCoords.z)) * 0.1;
		float tmp = (sin(worldCoords.x + u_WaterWave) + sin(worldCoords.z + u_WaterWave)) * 0.1 - 0.3;
		worldCoords = worldCoords + vec4(0, tmp, 0, 0);
		gl_Position = u_ViewProj * worldCoords;
	}

	Normal = GetNormal();
	texturePos = GetTexturePos();
};

#shader fragment
#version 330 core

//Base
in vec2 texturePos;
in vec3 Normal;
in float textureType;

uniform sampler2D u_TextureGrass;
uniform sampler2D u_TextureWater;
uniform sampler2D u_TextureSand;
uniform sampler2D u_TextureDirt;
uniform sampler2D u_TextureSnow;

//light
in float range;
in vec3 fragPos;
uniform vec4 u_posLight;
uniform float u_LightDistance;

out vec4 out_Color; //was gl_Color

void main()
{
	float val = 1.0 - range / u_LightDistance; //light distance = 0 - 40

	vec4 texColor; //texture color
	if (textureType < 0.5f)
		texColor = texture(u_TextureGrass, texturePos);
	else if (textureType < 1.5f)
		texColor = texture(u_TextureWater, texturePos);
	else if (textureType < 2.5f)
		texColor = texture(u_TextureSand, texturePos);
	else if (textureType < 3.5f)
		texColor = texture(u_TextureDirt, texturePos);
	else if (textureType < 4.5f)
		texColor = texture(u_TextureSnow, texturePos);

	if (texColor.a < 0.01)
		discard;
	
	float ambientStrength = 0.4f; // 0.1
	vec3 lightColor = vec3(0.7, 0.7, 0.7);
	vec3 ambient = ambientStrength * lightColor;
	
	// Diffuse 
	vec3 norm = Normal;
	vec3 LightVector = normalize(u_posLight.xyz - fragPos);
	float diff = max(dot(norm, LightVector), 0.0);
	vec3 diffuse = (diff * lightColor);

	vec3 result = 2.0 * val * (ambient + diffuse) * texColor.xyz;
	
	//for (int i = 0; i < 200000; i++) //PERFORMANCE TEST
	//	gl_Color = vec4(result + i, 1.0f);

	out_Color = vec4(result, texColor.w);
	//if (distance(fragPos, vec3(0, 150, 0)) > 50.f) //TODO FOG AT DISTANCE
	//	gl_Color = vec4(result, texColor.w - 0.5);
};