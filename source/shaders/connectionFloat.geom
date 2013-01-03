#version 400

uniform mat4 View, Projection;
uniform samplerBuffer Inst_Translation;
uniform samplerBuffer Inst_Attribute;

uniform vec3 CameraDirection;
uniform float ConnectionWidth;
uniform vec3 CameraPosition;
uniform float FogStart, FogEnd;

flat in uint Vert_ID[1];
flat in uint Vert_Neuron_IN[1];
flat in uint Vert_Neuron_OUT[1];

flat out uint ID;
out float Value;
out float Depth;
out vec3 WorldPos, Normal;

layout (points) in;
layout (triangle_strip,max_vertices =4) out;

void main( void )
{
    int inNeuronIndex = int(Vert_Neuron_IN[0])-1;
    int outNeuronIndex = int(Vert_Neuron_OUT[0])-1;

    vec3 inPosition = texelFetch(Inst_Translation,inNeuronIndex).xyz;
    vec3 outPosition = texelFetch(Inst_Translation,outNeuronIndex).xyz;

    vec3 lineDir = normalize(outPosition - inPosition);
    vec3 tangent =  normalize(cross(CameraDirection,lineDir));
    Normal = normalize(cross(tangent,lineDir));
    vec3 distanceToMove = (tangent * ConnectionWidth/2);
    mat4 wvp = Projection * View;

    ID = Vert_ID[0];

    Depth = min((distance(CameraPosition,inPosition) - FogStart) / (FogEnd - FogStart),1.0f);
    Value = texelFetch(Inst_Attribute,inNeuronIndex).r;

    WorldPos = inPosition- distanceToMove;
    gl_Position = wvp *vec4(WorldPos,1.0f);
    EmitVertex();

    WorldPos = inPosition+ distanceToMove;
    gl_Position = wvp *vec4(WorldPos,1.0f);
    EmitVertex();


    Depth = min((distance(CameraPosition,outPosition) - FogStart) / (FogEnd - FogStart),1.0f);
    Value = texelFetch(Inst_Attribute,outNeuronIndex).r;

    WorldPos = outPosition- distanceToMove;
    gl_Position = wvp *vec4(WorldPos,1.0f);
    EmitVertex();

    WorldPos = outPosition+ distanceToMove;
    gl_Position = wvp *vec4(WorldPos ,1.0f);
    EmitVertex();

    EndPrimitive();
}

