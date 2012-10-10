#version 410

uniform vec3 WorldSize;
uniform vec3 CameraPosition;
uniform vec3 LightPosition;
uniform vec3 LightColor;
uniform float AmbientIntensity;
uniform float MaterialSpecularIntensity;
uniform float SpecularPower;

uniform sampler2D DiffuseMap;
uniform sampler2D NormalMap;
uniform sampler2D PositionMap;

in vec2 TextureCoordinate;
out vec4 Color;
void main()
{
    vec4 AmbientColor = vec4(LightColor, 1.0f) * AmbientIntensity;
    vec3 startPos = - WorldSize / 2.0f;
    vec3 position = startPos + texture2D(PositionMap,TextureCoordinate) * WorldSize;
    vec3 LightDirection = - (LightPosition - position);
    vec3 Normal = normalize(texture2D(NormalMap,TextureCoordinate));

    float DiffuseFactor = dot(Normal, LightDirection);

    vec4 DiffuseColor = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0)
    {
        DiffuseColor = vec4(LightColor, 1.0f) * DiffuseFactor;

        vec3 VertexToEye = normalize(CameraPosition - position);
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        SpecularFactor = pow(SpecularFactor, SpecularPower);
        if (SpecularFactor > 0) {
            SpecularColor = vec4(LightColor, 1.0f) * MaterialSpecularIntensity * SpecularFactor;
        }
    }

    Color = texture2D(DiffuseMap,TextureCoordinate) * (AmbientColor + DiffuseColor + SpecularColor);
}
