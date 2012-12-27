#version 400

const int MAX_LIGHTS = 5;
uniform float DiffuseIntensity;
uniform float AmbientIntensity;
uniform float SpecularIntensity;

struct PointLight
{
    vec3 Color;
    vec3 Position;
    float SpecularPower;
    float Radius;
    float ConstantAttenuation;
    float LinearAttenuation;
    float QuadraticAttenuation;
};
uniform PointLight Lights[MAX_LIGHTS];
uniform int NumLights;
uniform vec3 WorldCenter;
uniform vec3 WorldSize;
uniform vec3 CameraPosition;

uniform sampler2D DiffuseMap;
uniform sampler2D NormalMap;
uniform sampler2D PositionMap;
uniform usampler2D IdentityMap;

in vec2 TextureCoordinate;
out vec4 Color;
/*
void main()
{
    vec3 dColor = texture(DiffuseMap,TextureCoordinate).xyz;
    vec3 posScale =texture(PositionMap,TextureCoordinate).xyz ;
    vec3 position = WorldCenter + posScale* WorldSize - WorldSize/2.0f;
    vec3 Normal = texture(NormalMap,TextureCoordinate).xyz;

    vec3 lightingContribution = vec3(0,0,0);
    for (int i = 0; i < NumLights; i ++)
    {
        vec3 lightDirection = position -Lights[i].Position ;

        vec3 ambientColor =Lights[i].Color * AmbientIntensity;

        float diffuseFactor = dot(Normal, lightDirection);
        float distance = length(lightDirection)/ Lights[i].Radius;
        float attenuation = Lights[i].ConstantAttenuation + Lights[i].LinearAttenuation * distance + Lights[i].QuadraticAttenuation * distance * distance;
        vec3 diffuseColor = vec3(0, 0, 0);
        vec3 specularColor = vec3(0, 0, 0);

        if (DiffuseIntensity > 0)
        {
            diffuseColor = Lights[i].Color * DiffuseIntensity;
            vec3 vertexToEye = normalize(CameraPosition - position);
            vec3 lightReflect = normalize(reflect(lightDirection, Normal));
            float specularFactor = dot(vertexToEye, lightReflect);
            specularFactor = pow(specularFactor, Lights[i].SpecularPower);
            if (specularFactor > 0) {
                specularColor = Lights[i].Color * SpecularIntensity * specularFactor;
            }
        }
        lightingContribution +=(ambientColor + diffuseColor + specularColor)/3/attenuation;
    }
    Color = vec4( dColor + lightingContribution/NumLights,1.0);

}
*/

void main ()
{

    bool diffuseIsItem = texture(IdentityMap,TextureCoordinate).r > 0;
    vec3 sceneColor = texture(DiffuseMap,TextureCoordinate).xyz;
    vec3 posScale =texture(PositionMap,TextureCoordinate).xyz ;
    vec3 position = WorldCenter + posScale* WorldSize - WorldSize/2.0f;
    vec3 normal = normalize(texture(NormalMap,TextureCoordinate).xyz);

    // Get the eye vector
    vec3 eyeVector = normalize(position-CameraPosition);
    // Get the normal
    vec3 totalColoring = vec3(0,0,0);
    for (int i = 0; i < NumLights; i ++)
    {
        vec3 lightDirection = Lights[i].Position - position;
        // Calculate the lighting vector
        vec3 lightVector = normalize(lightDirection);
        // Calculate the diffuse coeficient based on normal X light angle
        float diffuseCoeficient = max(dot(normal,lightVector),0.0);
        // Multiply light and material color by diffuse coeficient
        vec3 diffuseColor = Lights[i].Color *DiffuseIntensity * diffuseCoeficient;
        // Add the products of light ambient by material ambient
        // and light scene ambient by material ambient
        vec3 ambientColor = Lights[i].Color * AmbientIntensity/5;
        // Calculate the half vector to be used by the specular coeficient
        // according to Blinn-Phong's model
        vec3 halfVector = normalize((eyeVector+lightVector)/2.0);
        // Calculate the specular coeficient using  the reflected and the eye vector
        // raise it to the power of the shininess parameter
        float specularCoeficient = pow(max(dot(normal,halfVector),0.0),Lights[i].SpecularPower);
        // Multiply light's specular by material's specular by the specular coeficient
        vec3 specularColor = vec3(0.0,0.0,0.0);
        // Specular reflections will show in the back of the object too
        // this trick allows us to ignore backfacing pixels (relative to light's position)
        float distance = length(lightDirection)/ Lights[i].Radius;
        float attenuation = Lights[i].ConstantAttenuation + Lights[i].LinearAttenuation * distance + Lights[i].QuadraticAttenuation * distance * distance;

        if(diffuseCoeficient > 0.0)
            specularColor = Lights[i].Color *SpecularIntensity* specularCoeficient * diffuseCoeficient;
        totalColoring +=(diffuseColor + ambientColor + specularColor)/attenuation;
    }
    // Add all the elements
    if (diffuseIsItem)
        Color = vec4(sceneColor/2.0f + totalColoring/2.0f,1.0f);
    else
        Color = vec4(sceneColor,1.0f);
}
