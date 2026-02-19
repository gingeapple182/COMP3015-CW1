#version 460

in vec3 Position;
in vec3 Normal;

layout (location = 0) out vec4 FragColor;

uniform struct LightInfo{
    vec4 Position;
    vec3 La;
    vec3 L;
}lights[3];

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

const int levels = 5;
const float scaleFactor = 1.0/levels;

//can get rid of this section if brave enough
vec3 phongModel(int light, vec3 position, vec3 n){
    vec3 ambient = lights[light].La * Material.Ka;
    vec3 s = normalize(vec3(lights[light].Position.xyz) - position);
    float sDotN = max(dot(s, n), 0.0);
    vec3 diffuse = Material.Kd * sDotN;
    vec3 spec = vec3(0.0);
    if (sDotN > 0.0){
        vec3 v = normalize(-position.xyz);
        vec3 r = reflect(-s, n);
        spec = Material.Ks * pow(max(dot(r, v), 0.0), Material.Shininess);
    }
    return ambient + (diffuse + spec) * lights[light].L;
}

vec3 blinnPhongSpot(vec3 position, vec3 n)
{
    vec3 ambient = Spot.La * Material.Ka;
    
    vec3 s = normalize(vec3(Spot.Position.xyz-position));

    vec3 diffuse = vec3(0.0f);

    vec3 spec = vec3(0.0f);

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

void main() {
	//vec3 Colour = vec3(0.0);
    //for (int i = 0; i <3; i++)
    //    Colour += phongModel(i, Position, Normal);
    vec3 shadeColour = blinnPhongSpot(Position, normalize(Normal));
    vec3 Colour = shadeColour;
    FragColor = vec4(Colour, 1.0);
}
