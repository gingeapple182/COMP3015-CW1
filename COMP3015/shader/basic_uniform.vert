#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;

out vec3 LightDir;
out vec3 ViewDir;
out vec2 TexCoord;

uniform struct LightInfo{
    vec4 Position;
    vec3 La;
    vec3 L;
}Light;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

out vec3 LightIntensity, n, pos;

void main()
{
	//vec3 normal = normalize(NormalMatrix * VertexNormal);
	//vec3 tangent = normalize(NormalMatrix * vec3(VertexTangent));
	//vec3 binormal = normalize(cross(normal, tangent)) * VertexTangent.w;
	//vec3 Position = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;

	//mat3 toObjectLocal = mat3(
	//	tangent.x, binormal.x, normal.x,
	//	tangent.y, binormal.y, normal.y,
	//	tangent.z, binormal.z, normal.z
	//);
	

	//LightDir = toObjectLocal * (Light.Position.xyz - Position);
	//ViewDir = toObjectLocal * normalize(-Position);
	//TexCoord = VertexTexCoord;

	//gl_Position = MVP * vec4(VertexPosition, 1.0);
	vec3 N = normalize(NormalMatrix * VertexNormal);
	vec3 T = normalize(NormalMatrix * vec3(VertexTangent.xyz));

	// Make T orthogonal to N (helps a LOT with noisy normal maps)
	T = normalize(T - N * dot(N, T));

	vec3 B = cross(N, T) * VertexTangent.w;
	
	// Columns are T, B, N
	mat3 TBN = mat3(T, B, N);

	mat3 invTBN = transpose(TBN);

	vec3 Position = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;

	vec3 lightDir;
	if (Light.Position.w == 0.0)
		lightDir = normalize(mat3(ModelViewMatrix) * Light.Position.xyz);       // directional
	else
		lightDir = Light.Position.xyz - Position;        // point light
	LightDir = invTBN * lightDir;
	//LightDir = TBN * (Light.Position.xyz - Position);

	ViewDir  = invTBN * normalize(-Position);
	TexCoord = VertexTexCoord;

	gl_Position = MVP * vec4(VertexPosition, 1.0);
}
