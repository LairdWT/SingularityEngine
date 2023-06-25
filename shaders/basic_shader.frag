#version 460

// input
// layout(location = 1) in vec3 vertexColor;

layout(push_constant) uniform push 
{
	mat2 transform;
	vec2 offset;
	vec3 color;
} Push;

// output
layout(location = 0) out vec4 outColor;

void main() 
{
	outColor = vec4(Push.color, 1.0);
};