#version 410

uniform mat4x4  WVP;
in vec3 vertex;
in float id;
out float ID;

void main( void )
{
    ID = id;
    gl_Position = WVP * vec4(vertex.xyz,1.0f);
}
