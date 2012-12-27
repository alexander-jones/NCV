#version 400
uniform usampler2D SelectionMap;
uniform sampler2D PositionMap;

uniform vec3 CameraPosition;

uniform int Time;
uniform float Sigma;
uniform float BlurSize;
uniform vec3 SelectionColor;

const float pi = 3.14159265f;
const int BlurBurstTime = 3000;
const float BlurBurstMagnitude = 1.5f;

in vec2 TextureCoordinate;
out vec4 Color;


void main()
{

    vec3 position = texture(PositionMap,TextureCoordinate).xyz;
    float distance = length(CameraPosition - position);

    vec3 incrementalGaussian;
    incrementalGaussian.x = 1.0f / (sqrt(2.0f * pi) * Sigma);
    incrementalGaussian.y = exp(-0.5f / (Sigma * Sigma));
    incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;

    // Take the central sample first...
    bool edgeSample = false;
    uint centralID = texture(SelectionMap,TextureCoordinate).r;
    float centralColor = float(int(centralID > 0)) ;
    vec3 avgValue = centralColor* SelectionColor * incrementalGaussian.x;
    float coefficientSum = incrementalGaussian.x;
    incrementalGaussian.xy *= incrementalGaussian.yz;

    float burstFactor = (1.0f + (float(Time %BlurBurstTime)/  float(BlurBurstTime))*BlurBurstMagnitude);
    burstFactor = 1.0f;
    // Go through the remaining 8 vertical samples (4 on each side of the center)
    for (float i = 1.0f; i <= 4.0f; i++)
    {
        for (float x = -1.0f; x <= 1.0f; x += 0.2f)
          for (float y = -1.0f; y <= 1.0f; y +=  0.2f)
          {
              uint sampleID = texture(SelectionMap,TextureCoordinate + burstFactor  *BlurSize *vec2(x,y)).r;
              edgeSample = edgeSample || (sampleID !=centralID  && centralID == 0)  ;
              float sampleColor = float(int(sampleID > 0)) ;
              avgValue +=   sampleColor * SelectionColor * incrementalGaussian.x;
          }
      coefficientSum += 2 * incrementalGaussian.x;
      incrementalGaussian.xy *= incrementalGaussian.yz;
    }
    if (edgeSample)
      Color = vec4(avgValue/coefficientSum,1.0);
    else
      Color = vec4(0,0,0,0);

}
