#version 330

uniform samplerBuffer Inst_Translation;
in uint Neuron_ID;

void main( void )
{
    int index = int(Neuron_ID) -1;
    gl_Position= vec4(texelFetch(Inst_Translation,index).xyz,1.0f);
}
