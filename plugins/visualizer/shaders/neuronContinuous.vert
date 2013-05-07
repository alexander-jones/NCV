#version 330

uniform mat4 WVP, Scale;
uniform samplerBuffer Inst_Translation;
uniform samplerBuffer Inst_Attribute;
uniform float FarPlane,NearPlane;
uniform float DepthBias,FarBias;

in vec3 Vert_Position;
in uint Inst_ID;

flat out uint ID;
out float Value;
out float logz;

void main( void )
{
    ID = Inst_ID ;
    Value = texelFetch(Inst_Attribute,int(Inst_ID)-1 ).r ;
    vec3 translation = texelFetch(Inst_Translation,int(Inst_ID)-1).rgb ;
    vec4 translated = vec4(translation,0.0f) + Scale *  vec4( Vert_Position,1.0f);
    gl_Position = WVP *  vec4(translated.xyz,1.0f);
    logz = log(gl_Position.w*DepthBias + 1)*FarBias;
    gl_Position.z = (2*logz - 1)*gl_Position.w;
}
