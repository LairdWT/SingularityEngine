#version 460

// input
layout(location = 0) in vec3 vertexColor;

// output
layout(location = 0) out vec4 outColor;

layout(push_constant) uniform push 
{
	mat4 meshMatrix; // projection * view * model
	mat4 normalMatrix;
} Push;

void main() 
{
	outColor = vec4 (vertexColor, 1.0);
};