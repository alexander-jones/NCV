#version 330

#extension GL_ARB_conservative_depth : enable

layout(depth_less) out float gl_FragDepth;

uniform mat4 Projection;
uniform vec3 SilhouetteColor;
uniform float FarPlane,NearPlane;
uniform float SilhouetteMaxAlpha;
uniform float SilhouetteDepthMagnification;

in float logz;

out vec4 Color;


void main(void)
{
    gl_FragDepth = logz;
    float depth =  clamp(pow(gl_FragDepth,SilhouetteDepthMagnification),0.0f,1.0f);
    Color = vec4(SilhouetteColor,  clamp(1.0f - depth ,0.0f,1.0f) * SilhouetteMaxAlpha);
}
