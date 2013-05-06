#version 330

uniform mat4 WVP, Scale;
uniform samplerBuffer Inst_Translation;
uniform samplerBuffer Inst_Attribute;
uniform int BitsPerValue, States,BitMask;

in vec3 Vert_Position;
in uint Inst_ID;

flat out uint ID;
flat out uint Value;
out float Depth;

void main( void )
{
    ID = Inst_ID ;
    int offset = int(Inst_ID)-1;
    int startOffset = offset * BitsPerValue;
    Value = clamp(uint( texelFetch(Inst_Attribute,startOffset/8).r * 255.0) & (uint(BitMask) << uint(startOffset % 8)),0U,uint(States-1));
    int endOffset = startOffset + BitsPerValue;
    if ((endOffset % 8) < (startOffset % 8))
        Value += clamp(uint(texelFetch(Inst_Attribute,endOffset/8).r * 255.0) & (uint(BitMask) >> uint(BitsPerValue - (endOffset % 8))),0U,uint(States-1));

    vec3 translation = texelFetch(Inst_Translation,int(Inst_ID)-1).rgb ;
    vec4 translated = vec4(translation,0.0f) + Scale *  vec4( Vert_Position,1.0f);
    gl_Position = WVP * vec4(translated.xyz,1.0f);
}
