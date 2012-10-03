#version 410


uniform mat4x4 World, View, Projection, WVP;
highp uniform float highlightID;
highp in float ID;


vec3 unpackID(float f) {
    vec3 color;
    color.b = floor(f / 256.0 / 256.0);
    color.g = floor((f - color.b * 256.0 * 256.0) / 256.0);
    color.r = floor(f - color.b * 256.0 * 256.0 - color.g * 256.0);
    // now we have a vec3 with the 3 components in range [0..256]. Let's normalize it!
    return color / 256.0;
}

void main( void )
{
    if(highlightID == ID)
        gl_FragData[0] = vec4( 1,0.5f,1,0.0f);
    else
        gl_FragData[0] = vec4( 1,1,1,1.0f);

    gl_FragData[1] = vec4( unpackID(ID),1.0f);

    //gl_FragData[1] = vec4( 0,0,1,1.0f);
    //fragColor = vec4( 1,1,1,1.0f);
}
