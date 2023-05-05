#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in uint pointType;

out vec4 typeColor;
out float gl_PointSize;

uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * vec4(position, 1.0);

	switch (pointType)
	{
	case 0u:
		gl_PointSize = 3.0f;
		typeColor = vec4(1.0, 1.0, 1.0, 1.0);
		break;
	case 1u:
		gl_PointSize = 3.0f;
		typeColor = vec4(1.0, 1.0, 0.0, 1.0);
		break;
	case 2u:
		gl_PointSize = 3.0f;
		typeColor = vec4(1.0, 0.0, 0.0, 1.0);
		break;
	default:
		gl_PointSize = 3.0f;
		typeColor = vec4(0.5, 0.5, 0.5, 1.0);
	}
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 typeColor;

void main()
{
	color = typeColor;
};