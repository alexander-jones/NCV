#version 410
layout (location = 0)in vec3 position;
layout (location = 1)in vec2 texCoord;

out vec2 TextureCoordinate;
out vec4 Position;

void main( void )
{
    Position = position;
    TextureCoordinate = texCoord;
}
