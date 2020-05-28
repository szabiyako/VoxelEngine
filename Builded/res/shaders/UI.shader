#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
uniform mat4 u_Proj;
out vec4 pos;

void main()
{
	gl_Position = u_Proj * vec4(position.xy, 0.0, 1.0);
	pos = vec4(position.xy, 0.0, 1.0);
};

#shader fragment
#version 330 core

out vec4 out_Color; // was gl_Color;
in vec4 pos;

void main()
{
	float size = 0.0015;
	if (pos.x < size && pos.x > -size && pos.y < size && pos.y > -size)
		out_Color = vec4(1.0, 1.0, 1.0, 1.0f);
	else
		out_Color = vec4(0.1, 0.1, 0.1, 1.0f);
};