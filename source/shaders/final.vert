#version 410
layout (location = 0)in vec2 Vert_Position;
layout (location = 1)in vec2 Vert_UV;

out vec2 TextureCoordinate;

void main( void )
{
    gl_Position = vec4(Vert_Position,1.0f,1.0f);
    TextureCoordinate = Vert_UV;
}
