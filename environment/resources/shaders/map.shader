#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in uint pointId;

out vec4 typeColor;

uniform mat4 u_MVP;
uniform vec2 u_mouseCoords;
uniform vec2 u_mouseEpsilon;

void main()
{
	gl_Position = u_MVP * vec4(position, 1.0);
	typeColor = color;
	
	if (abs(gl_Position.x - u_mouseCoords.x) < u_mouseEpsilon.x && abs(gl_Position.y - u_mouseCoords.y) < u_mouseEpsilon.y)
	{
		typeColor = vec4(0.0, 1.0, 1.0, 1.0);
	}
	gl_PointSize = 4.0f;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 typeColor;

void main()
{
	color = typeColor;
};