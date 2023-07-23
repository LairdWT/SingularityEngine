#version 460

// input
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec2 texCoord;

// output
layout(location = 0) out vec3 vertexColorOut;

layout(set = 0, binding = 0) uniform globalUniformBufferObject
{
	mat4 projectionViewMatrix;
	vec4 directionToLight;
} uniformBufferObject;

layout(push_constant) uniform push 
{
	mat4 meshMatrix; // projection * view * model
	mat4 normalMatrix;
} Push;

// output
// layout(location = 1) out vec3 vertexColorOut;

const float AMBIENT_INTENSITY = 0.04f;

void main() 
{
	gl_Position = uniformBufferObject.projectionViewMatrix * Push.meshMatrix * vec4(position, 1.0f);

	vec3 normalWorldSpace = normalize(mat3(Push.normalMatrix) * normals);
	float lightIntensity = AMBIENT_INTENSITY + max(dot(normalWorldSpace, uniformBufferObject.directionToLight.xyz), 0.0);

	vertexColorOut = lightIntensity * vertexColor;
}