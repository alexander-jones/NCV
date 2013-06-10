#version 330

#extension GL_ARB_conservative_depth : enable

layout(depth_less) out float gl_FragDepth;

uniform sampler1D ColorMap;
uniform int Deselected;
uniform float MinimumValue, MaximumValue;

flat in uint ID;
in float Value;
in vec3 Normal;
in float logz;

out vec4 FragData;
flat out uint PickData;

void main( void )
{
    gl_FragDepth = logz;
    float samplePoint = min(1.0,max(0.001,(Value - MinimumValue)/(MaximumValue - MinimumValue)));

    vec3 color = texture(ColorMap,samplePoint).rgb;
    if (Deselected > 0 )
        FragData = vec4(color/3,1.0f);
    else
        FragData = vec4(color,1.0f);


    PickData  = ID;
}
