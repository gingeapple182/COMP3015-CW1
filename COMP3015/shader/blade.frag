#version 460

in vec2 vUV;
in vec3 vNormalVS;
in vec3 vPosVS;

layout (location = 0) out vec4 FragColor;

// You can keep binding 0 + white.jpg, but shader works even if the texture is just white.
layout (binding = 0) uniform sampler2D BladeTex;

// Tweakables
uniform float CoreIntensity;     // e.g. 5.0
uniform float GlowIntensity;     // e.g. 7.5
uniform float RimPower;          // e.g. 2.5  (lower = thicker glow, higher = tighter glow)
uniform float GlowBase;          // e.g. 0.15 (adds a little glow everywhere)

void main()
{
	// Base texture (optional; white.jpg means it's basically 1.0 anyway)
	vec3 tex = texture(BladeTex, vUV).rgb;

	// View direction in view-space (camera at origin in view-space)
	vec3 V = normalize(-vPosVS);
	vec3 N = normalize(vNormalVS);

	// Fresnel/rim term: 0 when facing camera, 1 at silhouette
	float rim = pow(1.0 - max(dot(N, V), 0.0), RimPower);

	// Define colours
	vec3 coreCol = vec3(1.0);                 // blade itself = white
	vec3 glowCol = vec3(1.0, 0.0, 1.0);       // magenta glow

	// Core emissive + magenta halo
	vec3 core = coreCol * CoreIntensity;
	vec3 glow = glowCol * (GlowIntensity * (GlowBase + rim));

	// Combine; this will often exceed 1.0 which is fine for “emissive”.
	vec3 outCol = tex * core + glow;

	FragColor = vec4(outCol, 1.0);
}