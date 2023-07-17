#version 460

// input
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 normals;
layout(location = 3) in vec2 texCoord;

// output
layout(location = 0) out vec3 vertexColorOut;

layout(push_constant) uniform push 
{
	mat4 transform; // projection * view * model
	mat4 normalMatrix;
} Push;

// output
// layout(location = 1) out vec3 vertexColorOut;

const vec3 LIGHT_DIRECTION = normalize(vec3(1.0f, -3.0f, -1.0f));
const float AMBIENT_INTENSITY = 0.08f;

void main() 
{
	gl_Position = Push.transform * vec4(position, 1.0f);

	vec3 normalWorldSpace = normalize(mat3(Push.normalMatrix) * normals);
	float lightIntensity = AMBIENT_INTENSITY + max(dot(normalWorldSpace, LIGHT_DIRECTION), 0.0);

	vertexColorOut = lightIntensity * vertexColor;
}