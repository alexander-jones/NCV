#version 400

uniform mat4 View, Projection, Scale;
uniform samplerBuffer Inst_Translation;
uniform usamplerBuffer Inst_Selection;
in vec3 Vert_Position;
in vec3 Vert_Normal;
in uint Inst_ID;

out vec3 WorldPos;
out vec3 Normal;
flat out uint Selection;
flat out uint ID;
void main( void )
{
    Normal = Vert_Normal;
    ID = Inst_ID ;
    vec3 translation = texelFetch(Inst_Translation,int(Inst_ID)-1).rgb ;
    vec4 translated = vec4(translation,0.0f) + Scale *  vec4( Vert_Position,1.0f);
    WorldPos = translated.xyz;
    Selection = texelFetch(Inst_Selection,int(Inst_ID)-1).r;
    gl_Position = Projection * View * translated;
}
