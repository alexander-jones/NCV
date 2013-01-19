#version 400

uniform mat4 View, Projection, Scale;
uniform samplerBuffer Inst_Translation;
uniform usamplerBuffer Inst_Attribute;
uniform vec3 CameraPosition;
uniform float FogStart, FogEnd;

in vec3 Vert_Position;
in vec3 Vert_Normal;
in uint Inst_ID;

flat out uint ID;
flat out uint BitFlag;
out vec3 Normal, WorldPos;
out float Depth;

void main( void )
{
    ID = Inst_ID ;
    BitFlag = texelFetch(Inst_Attribute,(int(Inst_ID)-1)/8).r & uint(1 <<((int(Inst_ID)-1) % 8));
    vec3 translation = texelFetch(Inst_Translation,int(Inst_ID)-1).rgb ;
    vec4 translated = vec4(translation,0.0f) + Scale *  vec4( Vert_Position,1.0f);
    WorldPos = translated.xyz;
    Normal = Vert_Normal;
    gl_Position = Projection * View * translated;
    Depth = min((distance(CameraPosition,translated.rgb) - FogStart) / (FogEnd - FogStart),1.0f);
}
