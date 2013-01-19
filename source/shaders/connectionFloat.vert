#version 400

uniform mat4 View;
uniform samplerBuffer Inst_Translation;
uniform samplerBuffer Inst_Attribute;
uniform vec3 CameraPosition;
uniform float FogStart, FogEnd;

in uint Neuron_ID;
in uint Inst_ID;

flat out uint Vert_ID;
out float Vert_Value;
out vec3 Vert_WorldPos;
out float Vert_Depth;

void main( void )
{

    int neuronIndex = int(Neuron_ID) -1;
    Vert_ID = Inst_ID;
    Vert_Value = texelFetch(Inst_Attribute,neuronIndex).r ;
    Vert_WorldPos = texelFetch(Inst_Translation,neuronIndex).xyz;
    Vert_Depth = min((distance(CameraPosition,Vert_WorldPos) - FogStart) / (FogEnd - FogStart),1.0f);
    gl_Position= View * vec4(Vert_WorldPos,1.0f);
}
