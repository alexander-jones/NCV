#version 410

const int  DIFFUSE = 0;
const int  POSITION = 1;
const int  NORMAL = 2;

uniform vec3 WorldSize ;

in vec3 Normal;
in vec3 WorldPos;

out vec4 FragData[3];

void main( void )
{


    vec3 startPos = - WorldSize / 2.0f;
    vec3 distColor = (WorldPos - startPos )/ WorldSize;

    FragData[DIFFUSE] = vec4( 0.2,0.2,0.5 ,1.0f);
    FragData[POSITION] = vec4( distColor,1.0f);
    FragData[NORMAL] = vec4(Normal,1.0f);

}
