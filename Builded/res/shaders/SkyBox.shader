#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texturepos;

out vec2 texturePos;

uniform mat4 u_Model;
uniform mat4 u_ViewProj;

void main()
{
	vec4 worldCoords = u_Model * vec4(position.xyz, 1.0);
	gl_Position = vec4(u_ViewProj * worldCoords).xyww;

	texturePos = texturepos;
};

#shader fragment
#version 330 core

in vec2 texturePos;
out vec4 out_Color; // was gl_Color;

uniform sampler2D u_Texture;


void main()
{
	out_Color = 0.7 * texture(u_Texture, texturePos);
};