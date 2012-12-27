#version 400
const int DIFFUSE = 0;
const int POSITION = 1;
const int NORMAL = 2;

const float e = 2.71828;
uniform vec4 DeselectionColor;
uniform int Deselected;
uniform vec3 OffColor;
uniform vec3 OnColor;
uniform vec3 WorldSize, WorldCenter;

flat in uint ID;
flat in uint BitFlag;
in vec3 WorldPos, Normal;
in float Depth;

out vec4 FragData[3];
flat out uint PickData;

void main( void )
{

    if (Deselected > 0 )
        FragData[DIFFUSE] = DeselectionColor;
    else if (BitFlag >0)
        FragData[DIFFUSE] =vec4(OnColor,1.0f);
    else
        FragData[DIFFUSE] =vec4(OffColor,1.0f);

    vec4 FogColor = vec4(0,0,0,1.0f);
    float intensity = pow(e,Depth*1.25f)/pow(e,1.25f);
    FragData[DIFFUSE] = intensity * FogColor + (1.0f- intensity) * FragData[DIFFUSE];


    vec3 startPos = - WorldSize / 2.0;
    vec3 distColor = (WorldPos - startPos )/ WorldSize;
    FragData[POSITION] = vec4(distColor,1.0f);
    FragData[NORMAL] = vec4(Normal,1.0f);

    PickData  = ID;
}
