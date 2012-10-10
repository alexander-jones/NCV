#version 410


 const int  DIFFUSE = 0;
 const int  POSITION = 1;
 const int  NORMAL = 2;
 const int  PICK = 3;


uniform vec3 WorldSize ;
uniform uint SelectedID;
in vec3 TextureCoordinate;
in vec3 Normal;
in vec3 WorldPos;
flat in uint ID;

out vec4 FragData[5];

vec3 unpackID(int id) {
    vec3 color;
    color.b = floor(id / 256.0 / 256.0);
    color.g = floor((id - color.b * 256.0 * 256.0) / 256.0);
    color.r = floor(id - color.b * 256.0 * 256.0 - color.g * 256.0);
    // now we have a vec3 with the 3 components in range [0..256]. Let's normalize it!
    return color / 256.0;
}


void main( void )
{


    vec3 startPos = - WorldSize / 2.0f;
    vec3 distColor = (WorldPos - startPos )/ WorldSize;

    if(SelectedID == ID)
        FragData[DIFFUSE] = vec4( 0.5,0.5f,1,0.0f);
    else
        FragData[DIFFUSE] = vec4( 0.2,0.2,0.5 ,1.0f);

    FragData[POSITION] = vec4( distColor,1.0f);
    FragData[NORMAL] = vec4(Normal,1.0f);
    FragData[PICK]  = vec4( unpackID(int(ID)),1.0f);

}
