#version 400

uniform samplerBuffer Inst_Translation;
uniform samplerBuffer Inst_Attribute;
uniform mat4 View, Projection;
uniform vec3 CameraPosition;
uniform float FogStart, FogEnd;

flat in uint Vert_Offset;
flat in uint Inst_Neuron_IN;
flat in uint Inst_Neuron_OUT;
flat in uint Inst_ID;


flat out uint Vert_ID;
out float Vert_Value;
out float Vert_Depth;
out vec3 Vert_Position;


void main( void )
{
    int index = int(Inst_Neuron_IN * (1-Vert_Offset) + Inst_Neuron_OUT * Vert_Offset)-1;

    Vert_Position = texelFetch(Inst_Translation,index).xyz;
    Vert_Depth = min((distance(CameraPosition,Vert_Position) - FogStart) / (FogEnd - FogStart),1.0f);

    Vert_ID = Inst_ID;
    Vert_Value = texelFetch(Inst_Attribute,index).r ;
    gl_Position = vec4(Vert_Position,1.0f);
}
