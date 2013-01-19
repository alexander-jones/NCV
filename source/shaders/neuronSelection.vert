#version 420

uniform mat4 Projection, View;
uniform vec3 Object_Bounds;

in uint Object_ID;
in vec3 Object_Position;

flat out uint ID;
out vec3 Position;

void main(void)
{
    ID = Object_ID;
    Position = Object_Position;
}
