#version 460

in vec3 SpotDir;
in vec3 LightDir;
in vec3 ViewDir;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;
layout (binding = 0) uniform sampler2D DiffuseTex;
layout (binding = 1) uniform sampler2D NormalMapTex;
layout (binding = 2) uniform sampler2D DiffuseTex2;

//uniform struct LightInfo{
//    vec4 Position;
//    vec3 La;
//    vec3 L;
//}lights[3];

uniform struct MaterialInfo{
    vec3 Kd;
    vec3 Ka;
    vec3 Ks;
    float Shininess;
}Material;

uniform struct SpotLightInfo{
    vec3 L;
    vec3 La;
    vec3 Position;
    vec3 Direction;
    float Exponent;
    float Cutoff;
}Spot;

uniform struct LightInfo{
    vec4 Position;
    vec3 La;
    vec3 L;
}Light;

uniform struct FogInfo{
    float MaxDistance;
    float MinDistance;
    vec3 Colour;
}Fog;

const int levels = 5;
const float scaleFactor = 1.0/levels;



//can get rid of this section if brave enough
//vec3 phongModel(int light, vec3 position, vec3 n){
//    vec3 ambient = lights[light].La * Material.Ka;
//    vec3 s = normalize(vec3(lights[light].Position.xyz) - position);
//    float sDotN = max(dot(s, n), 0.0);
//    vec3 diffuse = Material.Kd * sDotN;
//    vec3 spec = vec3(0.0);
//    if (sDotN > 0.0){
//        vec3 v = normalize(-position.xyz);
//        vec3 r = reflect(-s, n);
//        spec = Material.Ks * pow(max(dot(r, v), 0.0), Material.Shininess);
//    }
//    return ambient + (diffuse + spec) * lights[light].L;
//}



vec3 blinnPhong(vec3 n) {
    vec3 diffuse = vec3(0.0f); 
    vec3 spec = vec3(0.0f);
    
    vec3 texColour = texture(DiffuseTex, TexCoord).rgb;

    vec3 ambient = Light.La * texColour;
    vec3 s = normalize(LightDir);
    //vec3 s = normalize(Light.Position.xyz - position);

    float sDotN = max(dot(s, n), 0.0);

    diffuse = texColour * sDotN;

    if (sDotN > 0.0) {
	    vec3 v = normalize(ViewDir);
		vec3 h = normalize(v + s);
		spec = Material.Ks * pow(max(dot(h, n), 0.0), Material.Shininess);
	}
    return ambient + (diffuse + spec) * Light.L;
}



//can get rid of this section if brave enough -- dead function
vec3 blinnPhongSpot(vec3 position, vec3 n)
{
    vec3 diffuse = vec3(0.0f);
    vec3 spec = vec3(0.0f);
    
    vec3 ambient = Spot.La * Material.Ka;
    
    vec3 s = normalize(vec3(Spot.Position.xyz-position));

    float cosAng = dot(-s, normalize(Spot.Direction));
    float angle = acos(cosAng);
    float spotScale = 0.0;

    if (angle < Spot.Cutoff)
    {
        spotScale = pow(cosAng, Spot.Exponent);
        float sDotN = max(dot(n,s),0.0);
        diffuse = Material.Kd*floor(sDotN * levels) * scaleFactor;    
        

        if(sDotN > 0.0)
        {
            vec3 v = normalize(-position.xyz);
            vec3 r = reflect(-s,n);
            spec = Material.Ks * pow(max(dot(r,v), 0.0), Material.Shininess);
        }
    }

    return ambient + spotScale *Spot.L*(diffuse + spec);
}

vec3 blinnPhongSpotTS(vec3 n)
{
    vec3 texColour = texture(DiffuseTex, TexCoord).rgb;

    vec3 ambient = Spot.La * texColour;

    vec3 s = normalize(LightDir);  // tangent-space light dir
    vec3 v = normalize(ViewDir);

    //float cosAng = dot(normalize(-s), normalize(SpotDir));
    //float cutoffCos = cos(Spot.Cutoff);

    //if (cosAng <= cutoffCos)
    //    return ambient;
    float cosAng = dot(normalize(-s), normalize(SpotDir));

    float outer = cos(Spot.Cutoff);                 // outer cone edge (dark outside)
    float inner = cos(Spot.Cutoff * 0.85);          // inner cone edge (full bright inside)

    //float spotScale = smoothstep(outer, inner, cosAng);

    //spotScale = pow(spotScale, Spot.Exponent);

    float spotEdge = smoothstep(outer, inner, cosAng);
    float spotCore = pow(max(cosAng, 0.0), Spot.Exponent);
    float spotScale = spotEdge * spotCore;

    if (spotScale <= 0.001)
	    return ambient;

    //float spotScale = pow(cosAng, Spot.Exponent);

    float sDotN = max(dot(s, n), 0.0);
    vec3 diffuse = texColour * sDotN;

    vec3 spec = vec3(0.0);
    if (sDotN > 0.0) {
        vec3 h = normalize(v + s);
        spec = Material.Ks * pow(max(dot(h, n), 0.0), Material.Shininess);
    }

    return ambient + spotScale * Spot.L * (diffuse + spec);
}


void main() {
//fog stuff
    //float distance = abs(Position.z);
    //float fogFactor = (Fog.MaxDistance - distance) / (Fog.MaxDistance - Fog.MinDistance);
    //fogFactor = clamp(fogFactor, 0.0, 1.0);
    //vec3 shadeColour = blinnPhong(Position, normalize(Normal));
    //vec3 Colour = mix(Fog.Colour, shadeColour, fogFactor);  // -- remember to put Colour instead of shadeColour when i want fog back

//blinnphongspot - old
    //vec3 shadeColour = blinnPhongSpot(Position, normalize(Normal));
    //vec3 Colour = shadeColour;
    //FragColor = vec4(Colour, 1.0);

//blinnphong
    vec3 normal = texture(NormalMapTex, TexCoord).xyz;
    //normal.xy = 2.0 * normal.xy - 1.0;
    normal = normal * 2.0 - 1.0; 
    normal = normalize(normal);
    //FragColor = vec4(blinnPhong(normalize(normal)), 1.0);

//blinnphongspotts
    FragColor = vec4(blinnPhongSpotTS(normal), 1.0);
}
