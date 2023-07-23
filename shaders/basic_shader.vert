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
	vec4 ambientColor;
	vec4 lightPosition;
	vec4 lightColor;
} uniformBufferObject;

layout(push_constant) uniform push 
{
	mat4 meshMatrix; // projection * view * model
	mat4 normalMatrix;
} Push;

// output
// layout(location = 1) out vec3 vertexColorOut;

void main() 
{
	vec4 worldPosition = Push.meshMatrix * vec4(position, 1.0f);
	gl_Position = uniformBufferObject.projectionViewMatrix * worldPosition;

	vec3 normalWorldSpace = normalize(mat3(Push.normalMatrix) * normals);

	vec3 directionToLight = normalize(uniformBufferObject.lightPosition.xyz - worldPosition.xyz);
	float lightAttenuation = 1.0f / dot(directionToLight.xyz, directionToLight.xyz);
	vec3 lightColor = uniformBufferObject.lightColor.rgb * uniformBufferObject.lightColor.w * lightAttenuation;
	vec3 ambientLight = uniformBufferObject.ambientColor.rgb * uniformBufferObject.ambientColor.w;
	vec3 diffuseLight = lightColor * max(dot(normalWorldSpace, normalize(directionToLight)), 0.0f);

	vertexColorOut = vertexColor * (ambientLight + diffuseLight);
}