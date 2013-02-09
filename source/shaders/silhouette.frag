#version 400

uniform vec3 SilhouetteColor;
uniform float ZNear, ZFar, ZStop;

in float ZDepth;
out vec4 Color;

float linearize(float depth)
{
    return (2.0 * ZNear )/ (ZFar + ZNear - depth * (ZFar - ZNear));
}


void main(void)
{

    Color = vec4(SilhouetteColor,1.0f);//min(1.0f-(linearize(gl_FragCoord.z)/ZStop),1.0f));
}
