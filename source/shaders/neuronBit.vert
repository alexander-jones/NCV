#version 400

uniform mat4 WVP, Scale;
uniform samplerBuffer Inst_Translation;
uniform usamplerBuffer Inst_Attribute;
uniform vec3 CameraPosition;
uniform float FogStart, FogEnd;

in vec3 Vert_Position;
in vec3 Vert_Normal;
in uint Inst_ID;
in uint Custom_ID;

flat out uint ID;
flat out uint Value;
out vec3 Normal;
out float Depth;

void main( void )
{
    ID = Inst_ID ;
    Value = texelFetch(Inst_Attribute,(int(Inst_ID)-1)/32).r & uint(1 <<((int(Inst_ID)-1) % 32));
    vec3 translation = texelFetch(Inst_Translation,int(Inst_ID)-1).rgb ;
    vec4 translated = vec4(translation,0.0f) + Scale *  vec4( Vert_Position,1.0f);
    Normal = Vert_Normal;
    gl_Position = WVP * vec4(translated.xyz,1.0f);
    Depth = min((distance(CameraPosition,translated.xyz) - FogStart) / (FogEnd - FogStart),1.0f);
}
