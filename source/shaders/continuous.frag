#version 330

const float e = 2.71828;
uniform sampler1D RangeMap;
uniform vec4 DeselectionColor;
uniform int Deselected;
uniform float MinimumValue, MaximumValue;

flat in uint ID;
in float Value;
in vec3 Normal;

out vec4 FragData;
flat out uint PickData;

void main( void )
{
    float samplePoint = min(1.0,max(0.001,(Value - MinimumValue)/(MaximumValue - MinimumValue)));

    vec3 color = texture(RangeMap,samplePoint).rgb;
    if (Deselected > 0 )
        FragData = vec4(color/3,1.0f);
    else
        FragData = vec4(color,1.0f);


    PickData  = ID;
}
