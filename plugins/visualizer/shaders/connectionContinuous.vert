#version 330

uniform samplerBuffer Inst_Translation;
uniform samplerBuffer Inst_Attribute;
uniform int ConnectionIDStart;

in uint Neuron_ID;
in uint Inst_ID;

flat out uint Vert_ID;
out float Vert_Value;

void main( void )
{
    int neuronIndex = int(Neuron_ID) -1;
    int connectionIndex = int(Inst_ID  )- ConnectionIDStart;
    Vert_ID = Inst_ID;
    Vert_Value = texelFetch(Inst_Attribute,connectionIndex ).r ;
    gl_Position= vec4(texelFetch(Inst_Translation,neuronIndex).xyz,1.0f);
}
