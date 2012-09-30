#version 410


uniform mat4x4 World, View, Projection, WVP;
uniform int highlightID;
in float ID;
out vec4 fragColor;

void main( void )
{
    if(highlightID == ID)
        fragColor = vec4( 1,0.5f,1,0.0f);
    else

        fragColor = vec4( 1,1,1,1.0f);
    //fragColor = vec4( 1,1,1,1.0f);
}
