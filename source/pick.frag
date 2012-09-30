#version 410


uniform mat4x4 WVP;
in float ID;
out vec4 fragColor;

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
    fragColor = vec4( unpackID(ID),1.0f);
    //fragColor = vec4( 1,1,1,1.0f);
}
