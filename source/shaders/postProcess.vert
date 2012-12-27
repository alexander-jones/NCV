#version 400
in vec3 Vert_Position;
in vec2 Vert_Coord;

out vec2 TextureCoordinate;

void main( void )
{
    TextureCoordinate = Vert_Coord;
    gl_Position = vec4(Vert_Position,1.0f);
}
