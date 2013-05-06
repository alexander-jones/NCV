#version 330

uniform mat4 Projection;
uniform vec3 SilhouetteColor;
uniform float ZNear, ZFar, ZStop;
uniform float MaxAlpha;
uniform float DepthExponent;
out vec4 Color;

float linearize(float depth)
{
    return (2.0 * ZNear )/ (ZFar + ZNear - depth * (ZFar - ZNear));
}


void main(void)
{
    float cutoff = clamp((ZStop - ZNear) /(ZFar - ZNear),0.0f,1.0f);
    float depth =  clamp(pow(linearize(gl_FragCoord.z),DepthExponent),0.0f,cutoff);
    Color = vec4(SilhouetteColor,  clamp(1.0f - (depth /cutoff),0.0f,1.0f) * MaxAlpha);
}
