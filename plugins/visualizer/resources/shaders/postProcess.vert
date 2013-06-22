#version 330

in vec3 Vert_Position;
in vec2 Vert_Coord;
out vec2 TextureCoordinate;

void main(void)
{
    gl_Position = vec4(Vert_Position,1.0f);
    TextureCoordinate = Vert_Coord;
}
