#version 400
const int DIFFUSE = 0;
const int NORMAL = 1;

const float e = 2.71828;
uniform sampler1D RangeMap;
uniform vec4 DeselectionColor;
uniform int Deselected;
uniform float MinimumValue, MaximumValue;
uniform vec3 WorldSize, WorldCenter;

flat in uint ID;
in float Value;
in float Depth;
in vec3 Normal;

out vec4 FragData[2];
flat out uint PickData;

void main( void )
{
    float samplePoint = (Value - MinimumValue)/(MaximumValue - MinimumValue);

    vec3 color = texture(RangeMap,samplePoint).rgb;
    if (Deselected > 0 )
        FragData[DIFFUSE] = vec4(color/2,0.5f);
    else
        FragData[DIFFUSE] = vec4(color,1.0f);
    vec4 FogColor = vec4(0,0,0,1.0f);
    float intensity = pow(Depth*e,1.0f)/pow(e,1.0f);
    //FragData[DIFFUSE].rgb = intensity * FogColor.rgb + (1.0f- intensity) * FragData[DIFFUSE].rgb;

    FragData[NORMAL] = vec4(Normal,1.0f);

    PickData  = ID;
}
