#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texturepos;

uniform mat4 u_Model;
uniform mat4 u_ViewProj;

void main()
{
	vec4 worldCoords = u_Model * vec4(position.xyz, 1.0);
	gl_Position = u_ViewProj * worldCoords;
};

#shader fragment
#version 330 core

out vec4 out_Color; // was gl_Color;

void main()
{
	out_Color = vec4(1.0, 1.0, 1.0, 1.0f);
};