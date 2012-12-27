#version 400
uniform sampler2D BlendMap;

in vec2 TextureCoordinate;
out vec4 Color;

void main(void)
{
    Color = texture(BlendMap, TextureCoordinate);
}
