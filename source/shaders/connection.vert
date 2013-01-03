#version 400

uniform mat4 View, Projection;

flat in uint Inst_Neuron_IN;
flat in uint Inst_Neuron_OUT;
flat in uint Inst_ID;


flat out uint Vert_ID;
flat out uint Vert_Neuron_IN;
flat out uint Vert_Neuron_OUT;

void main( void )
{

    Vert_ID = Inst_ID;
    Vert_Neuron_IN = Inst_Neuron_IN;
    Vert_Neuron_OUT =Inst_Neuron_OUT;
}
