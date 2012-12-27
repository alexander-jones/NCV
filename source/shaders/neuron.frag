#version 400

const float e = 2.71828;
const int  MAP_DIFFUSE = 0;
const int  MAP_VOLTAGE = 1;
const int  MAP_FIRINGS = 2;
const int  MAP_POSITION = 3;
const int  MAP_NORMAL = 4;
const int  MAP_ID = 0;

uniform vec4 DeselectionColor;
uniform int Deselected;
uniform float MaximumValue,MinimumValue;
uniform vec3 OnColor,OffColor;

uniform vec3 WorldSize, WorldCenter, SelectionColor ;
uniform vec2 ScreenSize;

flat in uint ID;
in vec3 WorldPos;
in float Voltage;
in vec3 Normal;
flat in uint Firing;
in float Depth;

out vec4 FragData[5];
flat out uint PickData;

// map value in range from blue (low)  to green to yellow to red to white (high)
//based on intensity
vec4 intensityMap(float hMin, float hMax, float t)
{
    t = clamp(t, hMin, hMax);

    float invMaxH = 1.0 / (hMax - hMin);
    float zRel = (t - hMin) * invMaxH;

    float red = 0, green = 0, blue = 0;


    if (0 <= zRel && zRel < 0.2)
    {
        blue = 1.0;
        green = zRel * 5.0;
    }
    else if (0.2f <= zRel && zRel < 0.4)
    {
        green = 1.0;
        blue = 1.0 - (zRel - 0.2) * 5.0;
    }
    else if (0.4f <= zRel && zRel < 0.6)
    {
        green = 1.0;
        red = (zRel - 0.4) * 5.0;
    }
    else if (0.6f <= zRel && zRel < 0.8)
    {
        red = 1.0;
        green = 1.0 - (zRel - 0.6) * 5.0;
    }
    else
    {
        red = 1.0;
        green = (zRel - 0.8) * 5.0;
        blue = green;
    }

    return vec4(red, green, blue, 1.0);
}
void main( void )
{
    vec3 startPos = - WorldSize / 2.0;
    vec3 distColor = (WorldPos - startPos )/ WorldSize;
    if (Deselected > 0 )
        FragData[MAP_DIFFUSE] = DeselectionColor;
    else
        FragData[MAP_DIFFUSE] = vec4( normalize( vec3(ID % 6,ID % 5,ID % 4)).xyz,0.2f);


    float FogDensity = 1.0f;
    vec4 FogColor = vec4(0,0,0,1.0f);
    float intensity = FogDensity * pow(e,Depth*2)/pow(e,2);
    FragData[MAP_DIFFUSE] = intensity * FogColor + (1.0f- intensity) * FragData[MAP_DIFFUSE];

    if (Deselected > 0 )
        FragData[MAP_VOLTAGE] = DeselectionColor;
    else
        FragData[MAP_VOLTAGE] = vec4( intensityMap(MinimumValue,MaximumValue,Voltage).xyz,0.2f);

    if (Deselected > 0 )
        FragData[MAP_FIRINGS] = DeselectionColor;
    else if (Firing > 0)
        FragData[MAP_FIRINGS] = vec4(OnColor,1.0);
    else
        FragData[MAP_FIRINGS] = vec4(OffColor,1.0);

    FragData[MAP_POSITION] = vec4( distColor ,1.0);
    FragData[MAP_NORMAL] = vec4( Normal ,1.0);


    PickData  = ID;
}
