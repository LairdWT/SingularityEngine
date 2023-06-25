#version 460

// input
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 vertexColor;

layout(push_constant) uniform push 
{
	mat2 transform;
	vec2 offset;
	vec3 color;
} Push;

// output
// layout(location = 1) out vec3 vertexColorOut;

void main() 
{
	gl_Position = vec4(Push.transform * position + Push.offset, 0.0, 1.0);
}