#version 330
uniform mat4x4 World, View, Projection;

layout(location = 0, index = 0) out vec4 fragColor;

void main( void )
{
    fragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
}
