#version 400


uniform mat4 View, Projection;

uniform samplerBuffer Inst_Translation;
uniform samplerBuffer Inst_Voltage;
uniform usamplerBuffer Inst_Selection;
uniform usamplerBuffer Inst_Firing;

flat in uint Connection_ID[1];
flat in uint Neuron_IN[1];
flat in uint Neuron_OUT[1];

out vec3 WorldPos;
out float Voltage;

flat out uint Selection;
flat out uint NeuronSelection;
flat out uint ID;
flat out uint Firing;

layout (points) in;
layout (line_strip,max_vertices =2) out;

void main( void )
{
    Firing = texelFetch(Inst_Firing,int(ID)-1).r;
    Voltage = texelFetch(Inst_Voltage,int(Neuron_IN[0])-1).r;
    WorldPos = texelFetch(Inst_Translation,int(Neuron_IN[0])-1).xyz;
    ID = Connection_ID[0];
    NeuronSelection = max(texelFetch(Inst_Selection,int(Neuron_OUT[0])-1).r,texelFetch(Inst_Selection,int(Neuron_IN[0])-1).r);
    Selection = texelFetch(Inst_Selection,int(Connection_ID[0])-1).r;
    gl_Position = Projection * View * vec4(WorldPos,1.0f);
    EmitVertex();

    Firing = texelFetch(Inst_Firing,int(ID)-1).r;
    Voltage = texelFetch(Inst_Voltage,int(Neuron_OUT[0])-1).r;
    WorldPos = texelFetch(Inst_Translation,int(Neuron_OUT[0])-1).xyz;
    ID = Connection_ID[0];
    NeuronSelection = max(texelFetch(Inst_Selection,int(Neuron_OUT[0])-1).r,texelFetch(Inst_Selection,int(Neuron_IN[0])-1).r);
    Selection = texelFetch(Inst_Selection,int(Connection_ID[0])-1).r;
    gl_Position = Projection * View * vec4(WorldPos,1.0f);
    EmitVertex();

    EndPrimitive();
}
