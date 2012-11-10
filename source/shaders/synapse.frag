#version 400
const int  DIFFUSE = 0;
const int  POSITION = 1;
const int  VOLTAGE = 2;
const int  FIRINGS = 3;


const int NO_SELECTION = 0;
const int REGULAR_SELECTION = 1;
const int CONNECTIVITY_SELECTION = 2;


uniform float MaxVoltage,MinVoltage;
uniform vec3 FireColor,IdleColor;
uniform vec3 WorldSize, WorldCenter, SelectionColor, NueronSelectionColor ;
uniform float ColorIntensity;
uniform int SelectionState;

in vec3 WorldPos;
in float Voltage;
flat in uint Firing;
flat in uint ID;
flat in uint Selection;
flat in uint NeuronSelection;

out vec4 FragData[4];
out uint PickData;


// map value in range from blue (low)  to green to yellow to red to white (high)
//based on intensity
vec4 intensityMap(float hMin, float hMax, float t)
{
    t = clamp(t, hMin, hMax);

    float invMaxH = 1.0f / (hMax - hMin);
    float zRel = (t - hMin) * invMaxH;

    float red = 0, green = 0, blue = 0;


    if (0 <= zRel && zRel < 0.2f)
    {
        blue = 1.0f;
        green = zRel * 5.0f;
    }
    else if (0.2f <= zRel && zRel < 0.4f)
    {
        green = 1.0f;
        blue = 1.0f - (zRel - 0.2f) * 5.0f;
    }
    else if (0.4f <= zRel && zRel < 0.6f)
    {
        green = 1.0f;
        red = (zRel - 0.4f) * 5.0f;
    }
    else if (0.6f <= zRel && zRel < 0.8f)
    {
        red = 1.0f;
        green = 1.0f - (zRel - 0.6f) * 5.0f;
    }
    else
    {
        red = 1.0f;
        green = (zRel - 0.8f) * 5.0f;
        blue = green;
    }

    return vec4(red, green, blue, 1.0f);
}


void main( void )
{

    vec3 startPos = - WorldSize / 2.0f;
    vec3 distColor = (WorldPos - startPos )/ WorldSize;
    vec3 randColor =  normalize( vec3(ID % 1,ID % 2,ID % 7)).xyz;

    if (SelectionState == REGULAR_SELECTION && Selection > 0 )
        FragData[DIFFUSE] = vec4( ColorIntensity * SelectionColor ,1.0f);
    else if (SelectionState == CONNECTIVITY_SELECTION && NeuronSelection >0)
        FragData[DIFFUSE] = vec4( ColorIntensity * NueronSelectionColor ,1.0f);

    else
        FragData[DIFFUSE] = vec4(ColorIntensity * normalize( vec3(ID % 6,ID % 5,ID % 4)).xyz ,1.0f);

    if (SelectionState == REGULAR_SELECTION &&Selection > 0 )
        FragData[POSITION] = vec4(ColorIntensity *  SelectionColor ,1.0f);
    else if (SelectionState == CONNECTIVITY_SELECTION && NeuronSelection >0)
        FragData[POSITION] = vec4( ColorIntensity * NueronSelectionColor ,1.0f);
    else
        FragData[POSITION] = vec4(ColorIntensity * distColor,1.0f);

    if (SelectionState == REGULAR_SELECTION &&Selection > 0)
        FragData[VOLTAGE] = vec4(ColorIntensity *  SelectionColor ,1.0f);
    else if (SelectionState == CONNECTIVITY_SELECTION && NeuronSelection >0)
        FragData[VOLTAGE] = vec4( ColorIntensity * NueronSelectionColor ,1.0f);
    else
        FragData[VOLTAGE] = vec4(ColorIntensity * intensityMap(MinVoltage,MaxVoltage,Voltage).xyz,1.0f);

    if (SelectionState == REGULAR_SELECTION &&Selection > 0)
        FragData[FIRINGS] = vec4(ColorIntensity *  SelectionColor ,1.0f);
    else if (SelectionState == CONNECTIVITY_SELECTION && NeuronSelection >0)
        FragData[FIRINGS] = vec4( ColorIntensity * NueronSelectionColor ,1.0f);
    else if (Firing > 0)
        FragData[FIRINGS] = vec4(FireColor,1.0f);
    else
        FragData[FIRINGS] = vec4(IdleColor,1.0f);
    PickData  = ID;
}
