#version 330

uniform samplerBuffer ColorMap;

const float e = 2.71828;
uniform int Deselected;

flat in uint ID;
flat in uint Value;
in vec3 Normal;

out vec4 FragData;
flat out uint PickData;

void main( void )
{
    vec3 color = texelFetch(ColorMap,int(Value)).rgb;

    if (Deselected > 0 )
        FragData = vec4(color/3,1.0f);
    else
        FragData = vec4(color,1.0f);

    PickData  = ID;
}
