#version 330
const int DIFFUSE = 0;
const int NORMAL = 1;

const float e = 2.71828;
uniform sampler1D RangeMap;
uniform vec4 DeselectionColor;
uniform int Deselected;
uniform float MinimumValue, MaximumValue;

flat in uint ID;
in float Value;
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

    FragData[NORMAL] = vec4(Normal,1.0f);

    PickData  = ID;
}
