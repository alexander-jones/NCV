#version 330

const int DIFFUSE = 0;
const int NORMAL = 1;
uniform samplerBuffer ColorMap;

const float e = 2.71828;
uniform int Deselected;

flat in uint ID;
flat in uint Value;
in vec3 Normal;

out vec4 FragData[2];
flat out uint PickData;

void main( void )
{
    vec3 color = texelFetch(ColorMap,int(Value)).rgb;

    if (Deselected > 0 )
        FragData[DIFFUSE] = vec4(color/2,0.5f);
    else
        FragData[DIFFUSE] = vec4(color,1.0f);

    FragData[NORMAL] = vec4(Normal,1.0f);

    PickData  = ID;
}
