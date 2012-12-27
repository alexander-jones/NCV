#version 400

uniform mat4 View, Projection, Scale;
uniform samplerBuffer Inst_Translation;
uniform samplerBuffer Inst_Voltage;
uniform usamplerBuffer Inst_Firing;
uniform vec3 CameraPosition;
uniform float FogStart, FogEnd;

in vec3 Vert_Position;
in vec3 Vert_Normal;
flat in uint Inst_ID;

out vec3 WorldPos;
out vec3 Normal;
flat out uint Selection;
flat out uint ID;
flat out uint Firing;
out float Voltage;
out float Depth;
void main( void )
{
    ID = Inst_ID ;
    Normal = Vert_Normal;
    Firing = texelFetch(Inst_Firing,int(Inst_ID)-1).r;
    Voltage = texelFetch(Inst_Voltage,int(Inst_ID)-1).r;
    vec3 translation = texelFetch(Inst_Translation,int(Inst_ID)-1).rgb ;
    vec4 translated = vec4(translation,0.0f) + Scale *  vec4( Vert_Position,1.0f);
    WorldPos = translated.xyz;
    gl_Position = Projection * View * translated;
    Depth = (distance(CameraPosition,WorldPos) - FogStart) / (FogEnd - FogStart);
}
