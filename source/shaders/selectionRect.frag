#version 400
uniform vec3 SelectionColor;
out vec4 Color;

void main(void)
{
    Color =vec4(SelectionColor,1.0);
}
