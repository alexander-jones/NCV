#version 330

uniform mat4x4 World, View, Projection, WVP;
in vec4 vertex;

void main( void )
{
    gl_Position = WVP * vertex;
}
