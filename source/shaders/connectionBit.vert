#version 400

uniform samplerBuffer Inst_Translation;
uniform usamplerBuffer Inst_Attribute;
uniform uint ConnectionIDStart;

in uint Neuron_ID;
in uint Inst_ID;
in uint Custom_ID;

flat out uint Vert_ID;
flat out uint Vert_Value;

void main( void )
{

    int neuronIndex = int(Neuron_ID) -1;
    int connectionIndex = int(Inst_ID  - ConnectionIDStart);
    Vert_Value = texelFetch(Inst_Attribute,neuronIndex/32).r & (1 << neuronIndex % 32);
    Vert_ID = Inst_ID;
    gl_Position= vec4(texelFetch(Inst_Translation,neuronIndex).xyz,1.0f);
}
