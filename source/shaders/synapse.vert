#version 400

uniform samplerBuffer Inst_Translation;
uniform samplerBuffer Inst_Voltage;
uniform usamplerBuffer Inst_Firing;
uniform vec3 CameraDirection,CameraPosition;
uniform float FogStart, FogEnd;

flat in uint Vert_Offset;
flat in uint Inst_Neuron_IN;
flat in uint Inst_Neuron_OUT;
flat in uint Inst_ID;

out float Depth;
flat out uint Conn_ID;
flat out uint Conn_Firing;
out float Conn_Voltage;
out vec3 End_Position;

void main( void )
{
    int index = int(Inst_Neuron_IN * (1-Vert_Offset) + Inst_Neuron_OUT * Vert_Offset)-1;
   // int otherIndex = int(Inst_Neuron_IN * Vert_Offset + Inst_Neuron_OUT * (1-Vert_Offset))-1;

    End_Position = texelFetch(Inst_Translation,index).xyz;
    //vec3 otherPos = texelFetch(Inst_Translation,otherIndex).xyz;

    Conn_ID = Inst_ID;
    Conn_Firing = texelFetch(Inst_Firing,index).r;
    Conn_Voltage = texelFetch(Inst_Voltage,index).r;
    gl_Position = vec4(End_Position,1.0f);
    Depth = (distance(CameraPosition,End_Position) - FogStart) / (FogEnd - FogStart);
}
