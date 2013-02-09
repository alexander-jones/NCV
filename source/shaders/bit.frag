#version 400
const int DIFFUSE = 0;
const int NORMAL = 1;

const float e = 2.71828;
uniform vec4 DeselectionColor;
uniform int Deselected;
uniform vec3 OffColor;
uniform vec3 OnColor;
uniform vec3 WorldSize, WorldCenter;

flat in uint ID;
flat in uint Value;
in vec3  Normal;
in float Depth;

out vec4 FragData[2];
flat out uint PickData;

void main( void )
{
    vec3 color;
    if (Value >0)
        color = OnColor;
    else
        color = OffColor;

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
