#version 400
const int DIFFUSE = 0;
const int POSITION = 1;
const int NORMAL = 2;

const float e = 2.71828;
uniform vec4 DeselectionColor;
uniform int Deselected;
uniform float MinimumValue;
uniform float MaximumValue;
uniform sampler1D RangeMap;
uniform vec3 WorldSize, WorldCenter;

flat in uint ID;
in float Value;
in float Depth;
in vec3 WorldPos, Normal;

out vec4 FragData[3];
flat out uint PickData;

void main( void )
{
    float samplePoint = (Value - MinimumValue)/(MaximumValue - MinimumValue);
    if (Deselected > 0 )
        FragData[DIFFUSE] = DeselectionColor;
    else
        FragData[DIFFUSE] = vec4( samplePoint * vec3(0.5,0.25,0.75),1.0f);
        //FragData[DIFFUSE] = vec4(texture(RangeMap,samplePoint).rgb,1.0f);
    vec4 FogColor = vec4(0,0,0,1.0f);
    float intensity = pow(e,Depth*1.25f)/pow(e,1.25f);
    FragData[DIFFUSE] = intensity * FogColor + (1.0f- intensity) * FragData[DIFFUSE];

    vec3 startPos = - WorldSize / 2.0;
    vec3 distColor = (WorldPos - startPos )/ WorldSize;
    FragData[POSITION] = vec4(distColor,1.0f);
    FragData[NORMAL] = vec4(Normal,1.0f);

    PickData  = ID;
}
