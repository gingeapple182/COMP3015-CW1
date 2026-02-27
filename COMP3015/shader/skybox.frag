#version 460

layout (binding = 0) uniform samplerCube SkyboxTex;

layout (location = 0) out vec4 FragColour;

in vec3 Vec;

void main() {
	vec3 texColour = texture(SkyboxTex, normalize(Vec)).rgb;
	FragColour = vec4(texColour, 1.0);
}