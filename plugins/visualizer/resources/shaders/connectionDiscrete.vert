#version 330

uniform samplerBuffer Inst_Translation;
uniform samplerBuffer Inst_Attribute;
uniform int ConnectionIDStart;
uniform int BitsPerValue, States, BitMask;


in uint Neuron_ID;
in uint Inst_ID;

flat out uint Vert_ID;
flat out uint Vert_Value;

void main( void )
{
    int neuronIndex = int(Neuron_ID) -1;
    int connectionIndex = int(Inst_ID  )- ConnectionIDStart;
    int startOffset = connectionIndex * BitsPerValue;
    Vert_Value = clamp(uint(texelFetch(Inst_Attribute,startOffset/8).r * 255.0) & (uint(BitMask) << uint(startOffset % 8)),0U,uint(States-1));
    int endOffset = startOffset + BitsPerValue;
    if ((endOffset % 8) < (startOffset % 8))
        Vert_Value += clamp(uint(texelFetch(Inst_Attribute,endOffset/8).r * 255.0) & (uint(BitMask) >> uint(BitsPerValue - (endOffset % 8))),0U,uint(States-1));
    Vert_ID = Inst_ID;
    gl_Position= vec4(texelFetch(Inst_Translation,neuronIndex).xyz,1.0f);
}
