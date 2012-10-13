#version 410

uniform mat4 View, Projection, Scale;
layout (location = 0)in vec3 Vert_Position;
layout (location = 1)in vec3 Vert_Normal;
layout (location = 3)in mat4 Inst_Translation;

out vec3 WorldPos;
out vec3 Normal;

void main( void )
{
    vec4 translated = Inst_Translation * Scale *  vec4(Vert_Position.xyz,1.0f);
    WorldPos = translated.xyz;
    Normal = Vert_Normal;
    gl_Position = Projection * View * translated;
}
