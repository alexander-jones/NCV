#version 400

uniform mat4 View, Projection;
uniform samplerBuffer Inst_Translation;
uniform usamplerBuffer Inst_Attribute;
uniform vec3 CameraPosition;
uniform float FogStart, FogEnd;

flat in uint Vert_Offset;
flat in uint Inst_Neuron_IN;
flat in uint Inst_Neuron_OUT;
flat in uint Inst_ID;


flat out uint Vert_ID;
flat out uint Vert_BitFlag;
out float Vert_Depth;
out vec3 Vert_Position;


void main( void )
{
    int index = int(Inst_Neuron_IN * (1-Vert_Offset) + Inst_Neuron_OUT * Vert_Offset)-1;

    Vert_Position = texelFetch(Inst_Translation,index).xyz;
    Vert_Depth = min((distance(CameraPosition,Vert_Position) - FogStart) / (FogEnd - FogStart),1.0f);

    Vert_ID = Inst_ID;
    Vert_BitFlag = texelFetch(Inst_Attribute,index/8).r & uint(1 <<(index % 8));
    gl_Position = vec4(Vert_Position,1.0f);
}
