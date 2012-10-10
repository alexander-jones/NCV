#version 410

uniform mat4 View, Projection, Scale;
layout (location = 0)in vec3 position;
layout (location = 1)in vec3 normal;
layout (location = 2)in vec3 texCoord;
layout (location = 3)in mat4 Model;

flat out uint ID;
out vec3 WorldPos;
out vec3 Normal;
out vec3 TextureCoordinate;

void main( void )
{
    ID = gl_InstanceID + 1;
    vec4 translated = Model * Scale *  vec4(position.xyz,1.0f);
    WorldPos = translated.xyz;
    Normal = normal;
    TextureCoordinate = texCoord;
    gl_Position = Projection * View * translated;
}
