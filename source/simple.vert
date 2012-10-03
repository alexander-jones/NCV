#version 410

uniform mat4x4 World, View, Projection, WVP;
highp uniform float highlightID;

in vec3 vertex;
highp in float id;
highp out float ID;




void main( void )
{
    ID = id;
    gl_Position = WVP * vec4(vertex.xyz,1.0f);
}
