#version 400

uniform mat4 View, Projection, Scale;
uniform samplerBuffer Inst_Translation;
uniform samplerBuffer Inst_Attribute;
uniform vec3 CameraPosition;
uniform float FogStart, FogEnd;

in vec3 Vert_Position;
in vec3 Vert_Normal;
flat in uint Inst_ID;

flat out uint ID;
out float Value;
out float Depth;

void main( void )
{
    ID = Inst_ID ;
    Value = texelFetch(Inst_Attribute,int(Inst_ID)-1).r;
    vec3 translation = texelFetch(Inst_Translation,int(Inst_ID)-1).rgb ;
    vec4 translated = vec4(translation,0.0f) + Scale *  vec4( Vert_Position,1.0f);
    gl_Position = Projection * View * translated;
    Depth = min((distance(CameraPosition,translated.rgb) - FogStart) / (FogEnd - FogStart),1.0f);
}