#version 400

uniform mat4 View;
uniform samplerBuffer Inst_Translation;
uniform usamplerBuffer Inst_Attribute;
uniform vec3 CameraPosition;
uniform float FogStart, FogEnd;

in uint Neuron_ID;
in uint Inst_ID;

flat out uint Vert_ID;
flat out uint Vert_BitFlag;
out vec3 Vert_WorldPos;
out float Vert_Depth;

void main( void )
{

    int neuronIndex = int(Neuron_ID) -1;
    Vert_BitFlag = texelFetch(Inst_Attribute,neuronIndex/8).r & (1 << neuronIndex % 8);
    Vert_ID = Inst_ID;
    Vert_WorldPos = texelFetch(Inst_Translation,neuronIndex).xyz;
    Vert_Depth = min((distance(CameraPosition,Vert_WorldPos) - FogStart) / (FogEnd - FogStart),1.0f);
    gl_Position= View * vec4(texelFetch(Inst_Translation,neuronIndex).xyz,1.0f);
}
