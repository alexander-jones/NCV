#version 400

uniform mat4 View, Projection;
uniform vec3 CameraDirection;
uniform float ConnectionWidth;

flat in uint Vert_ID[2];
flat in uint Vert_BitFlag[2];
in float Vert_Depth[2];
in vec3 Vert_Position[2];

flat out uint ID;
flat out uint BitFlag;
out vec3 WorldPos, Normal;
out float Depth;

layout (lines) in;
layout (triangle_strip,max_vertices =4) out;

void main( void )
{
    vec3 lineDir = normalize(Vert_Position[1] - Vert_Position[0]);
    vec3 tangent =  normalize(cross(normalize(CameraDirection),lineDir));
    Normal = normalize(cross(tangent,lineDir));

    ID = Vert_ID[0];

    Depth = Vert_Depth[0];
    BitFlag = Vert_BitFlag[0];

    WorldPos = Vert_Position[0]- (tangent * ConnectionWidth/2);

    gl_Position = Projection * View *vec4(WorldPos,1.0f);
    EmitVertex();

    WorldPos = Vert_Position[0]+ (tangent * ConnectionWidth/2);
    gl_Position = Projection * View *vec4(WorldPos,1.0f);
    EmitVertex();

    Depth = Vert_Depth[1];
    BitFlag = Vert_BitFlag[1];

    WorldPos = Vert_Position[1]- (tangent * ConnectionWidth/2);
    gl_Position = Projection * View *vec4(WorldPos,1.0f);
    EmitVertex();

    WorldPos = Vert_Position[1]+ (tangent * ConnectionWidth/2);
    gl_Position = Projection * View *vec4(WorldPos ,1.0f);
    EmitVertex();

    EndPrimitive();
}
