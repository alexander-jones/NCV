#version 400

uniform mat4 View, Projection;
uniform vec3 CameraDirection;
uniform float ConnectionWidth;

flat in uint Conn_ID[2];
flat in uint Conn_Firing[2];
in float Conn_Voltage[2];
in vec3 End_Position[2];

out vec3 WorldPos;
out vec3 Normal;
out float Voltage;
flat out uint ID;
flat out uint Firing;

layout (lines) in;
layout (triangle_strip,max_vertices =4) out;

void main( void )
{
    vec3 lineDir = normalize(End_Position[1] - End_Position[0]);
    vec3 tangent =  normalize(cross(normalize(CameraDirection),lineDir));
    Normal = normalize(cross(lineDir,tangent));

    ID = Conn_ID[0];
    Firing = min(Conn_Firing[0] + Conn_Firing[1],1);

    Voltage = Conn_Voltage[0];

    WorldPos = End_Position[0]- (tangent * ConnectionWidth/2);
    gl_Position = Projection * View *vec4(WorldPos,1.0f);
    EmitVertex();

    WorldPos = End_Position[0]+ (tangent * ConnectionWidth/2);
    gl_Position = Projection * View *vec4(WorldPos,1.0f);
    EmitVertex();

    Voltage = Conn_Voltage[1];

    WorldPos = End_Position[1]- (tangent * ConnectionWidth/2);
    gl_Position = Projection * View *vec4(WorldPos,1.0f);
    EmitVertex();

    WorldPos = End_Position[1]+ (tangent * ConnectionWidth/2);
    gl_Position = Projection * View *vec4(WorldPos ,1.0f);
    EmitVertex();

    EndPrimitive();
}
