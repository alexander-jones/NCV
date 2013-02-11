#version 400
uniform usampler2D BlendMap;

in vec2 TextureCoordinate;
out vec4 Color;

vec4 hexToColor(uint hex)
{
    float alpha = float(hex >> 24U )/255.0;
    float red = float((hex >> 16U) & 255U) /255.0;
    float green = float((hex >> 8U)& 255U)/255.0;
    float blue = float(hex & 255U)/255.0;
    return vec4(red,green,blue,alpha);
}

void main(void)
{
    Color = hexToColor(texture(BlendMap, TextureCoordinate).r);
}
