#version 330
uniform sampler2D DepthTexture;
uniform sampler2D DiffuseTexture;
uniform vec2 ScreenSize;

in vec2 TextureCoordinate;
out vec4 Color;

float computeOcclusion( float occludedDepth, float occluderDepth, float mult , float scale)
{
    float aoCap = 1.0;
    float depthTolerance=0.000;
    float ao = min(aoCap,max(0.0,occludedDepth-occluderDepth-depthTolerance) * mult) /scale;
    return ao;
}

void main(void)
{
    float depth = texture2D(DepthTexture, TextureCoordinate).x;
    float ao = 0.0;
    int numSamples =4;

    for (int i = 1; i <= numSamples ; i++)
    {
        vec2 step = vec2((i *2.0f )/ ScreenSize.x,(i *2.0f ) / ScreenSize.y);
        float aoMultiplier=10000.0 / (i *2.0f );
        float aoScale=1.0 * (i *1.2f );

        for (int x = -1; x <= 1; x +=2)
        {
            for (int y = -1; y <= 1; y +=2)
            {
                vec2 dir = vec2(x,y);
                float d=texture2D(DepthTexture, TextureCoordinate + (step*dir)).x;
                ao+=computeOcclusion(depth,d,aoMultiplier,aoScale);
            }
        }


    }
    ao/= (numSamples *4.0);
    Color = vec4(1.0-ao) * texture2D(DiffuseTexture,TextureCoordinate);
}
