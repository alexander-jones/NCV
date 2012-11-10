#version 400

in uint Inst_Neuron_IN;
in uint Inst_Neuron_OUT;
in uint Inst_ID;

flat out uint Neuron_IN;
flat out uint Neuron_OUT;
flat out uint Connection_ID;

void main( void )
{
    Neuron_IN = Inst_Neuron_IN;
    Neuron_OUT = Inst_Neuron_OUT;
    Connection_ID = Inst_ID;
    gl_Position = vec4( 0.5,0.5,0.5,1.0f);
}
