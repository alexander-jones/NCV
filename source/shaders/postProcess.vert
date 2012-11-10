#version 400
in vec3 Vert_Position;

void main( void )
{
    gl_Position = vec4(Vert_Position,1.0f);
}
