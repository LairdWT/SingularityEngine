#version 460

// input
layout(location = 0) in vec3 vertexColor;

// output
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform push 
{
	mat4 transform;
	vec3 color;
} Push;

void main() 
{
	outColor = vec4(0.18, 0.18, 0.18, 1.0);
};