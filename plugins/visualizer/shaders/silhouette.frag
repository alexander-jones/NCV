#version 330

#extension GL_ARB_conservative_depth : enable

layout(depth_less) out float gl_FragDepth;

uniform mat4 Projection;
uniform vec3 SilhouetteColor;
uniform float FarPlane,NearPlane;
uniform float ZNear, ZFar, ZStop;
uniform float MaxAlpha;
uniform float DepthExponent;

in float logz;

out vec4 Color;

float linearize(float depth)
{
    return (2.0 * NearPlane )/ (FarPlane + NearPlane - depth * (FarPlane - NearPlane));
}


void main(void)
{
    gl_FragDepth = logz;
    float cutoff = clamp((ZStop - NearPlane) /(FarPlane - NearPlane),0.0f,1.0f);
    float depth =  clamp(pow(linearize(gl_FragDepth),DepthExponent),0.0f,cutoff);
    Color = vec4(SilhouetteColor,  clamp(1.0f - (depth /cutoff),0.0f,1.0f) * MaxAlpha);
}
