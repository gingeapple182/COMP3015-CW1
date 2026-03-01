#version 460

in vec3 vNormalVS;
in vec3 vPosVS;

layout (location = 0) out vec4 FragColor;

// Set from C++
uniform vec3 BladeColour;     // (1,1,1) for white
uniform float Intensity;      // brightness, e.g. 5..20
uniform float RimPower;       // e.g. 2.0..4.0
uniform float RimIntensity;   // e.g. 0.0..1.0 (extra edge pop)

void main()
{
	vec3 N = normalize(vNormalVS);
	vec3 V = normalize(-vPosVS);               // camera is origin in view-space

	float rim = pow(1.0 - max(dot(N, V), 0.0), RimPower);

	// Pure emissive core + optional rim boost
	vec3 col = BladeColour * Intensity;
	col += BladeColour * (rim * RimIntensity * Intensity);

	FragColor = vec4(col, 1.0);
}