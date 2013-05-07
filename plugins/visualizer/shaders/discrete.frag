#version 330

#extension GL_ARB_conservative_depth : enable

layout(depth_less) out float gl_FragDepth;

uniform samplerBuffer ColorMap;

const float e = 2.71828;
uniform int Deselected;

flat in uint ID;
flat in uint Value;
in vec3 Normal;
in float logz;

out vec4 FragData;
flat out uint PickData;

void main( void )
{
    gl_FragDepth = logz;
    vec3 color = texelFetch(ColorMap,int(Value)).rgb;

    if (Deselected > 0 )
        FragData = vec4(color/3,1.0f);
    else
        FragData = vec4(color,1.0f);

    PickData  = ID;
}
