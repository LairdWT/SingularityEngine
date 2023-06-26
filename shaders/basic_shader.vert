#version 460

// input
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColor;

// output
layout(location = 0) out vec3 vertexColorOut;

layout(push_constant) uniform push 
{
	mat4 transform;
	vec3 color;
} Push;

// output
// layout(location = 1) out vec3 vertexColorOut;

void main() 
{
	gl_Position = Push.transform * vec4(position, 1.0f);
	vertexColorOut = vertexColor;
}