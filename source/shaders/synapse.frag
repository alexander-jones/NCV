#version 400
const int  MAP_DIFFUSE = 0;
const int  MAP_POSITION = 1;
const int  MAP_VOLTAGE = 2;
const int  MAP_FIRINGS = 3;


const int NO_SELECTION = 0;
const int REGULAR_SELECTION = 1;
const int CONNECTIVITY_SELECTION = 2;


uniform float MaxVoltage,MinVoltage;
uniform vec3 FireColor,IdleColor;
uniform vec3 WorldSize, WorldCenter, SelectionColor, NeuronSelectionColor ;
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

    float invMaxH = 1.0 / (hMax - hMin);
    float zRel = (t - hMin) * invMaxH;

    float red = 0, green = 0, blue = 0;


    if (0 <= zRel && zRel < 0.2)
    {
        blue = 1.0;
        green = zRel * 5.0;
    }
    else if (0.2f <= zRel && zRel < 0.4)
    {
        green = 1.0;
        blue = 1.0 - (zRel - 0.2) * 5.0;
    }
    else if (0.4f <= zRel && zRel < 0.6)
    {
        green = 1.0;
        red = (zRel - 0.4) * 5.0;
    }
    else if (0.6f <= zRel && zRel < 0.8)
    {
        red = 1.0;
        green = 1.0 - (zRel - 0.6) * 5.0;
    }
    else
    {
        red = 1.0;
        green = (zRel - 0.8) * 5.0;
        blue = green;
    }

    return vec4(red, green, blue, 1.0);
}


void main( void )
{

    vec3 startPos = - WorldSize / 2.0;
    vec3 distColor = (WorldPos - startPos )/ WorldSize;
    vec3 randColor =  normalize( vec3(ID % 1,ID % 2,ID % 7)).xyz;
	
	if (Selection >0 && SelectionState == REGULAR_SELECTION)
		FragData[MAP_DIFFUSE] = vec4( SelectionColor,1.0);
	else if (Selection >0 && SelectionState == CONNECTIVITY_SELECTION)
		FragData[MAP_DIFFUSE] = vec4( NeuronSelectionColor,1.0);
	else
		FragData[MAP_DIFFUSE] = vec4( normalize( vec3(ID % 6,ID % 5,ID % 4)).xyz ,1.0);
		
	if (Selection >0 && SelectionState == REGULAR_SELECTION)
		FragData[MAP_POSITION] = vec4( SelectionColor,1.0);
	else if (Selection >0 && SelectionState == CONNECTIVITY_SELECTION)
		FragData[MAP_POSITION] = vec4( NeuronSelectionColor,1.0);
	else
		FragData[MAP_POSITION] = vec4( distColor,1.0);
	
	if (Selection >0 && SelectionState == REGULAR_SELECTION)
		FragData[MAP_VOLTAGE] = vec4( SelectionColor,1.0);
	else if (Selection >0 && SelectionState == CONNECTIVITY_SELECTION)
		FragData[MAP_VOLTAGE] = vec4( NeuronSelectionColor,1.0);
	else
		FragData[MAP_VOLTAGE] = vec4( intensityMap(MinVoltage,MaxVoltage,Voltage).xyz,1.0);

	if (Selection >0 && SelectionState == REGULAR_SELECTION)
		FragData[MAP_FIRINGS] = vec4( SelectionColor,1.0);
	else if (Selection >0 && SelectionState == CONNECTIVITY_SELECTION)
		FragData[MAP_FIRINGS] = vec4( NeuronSelectionColor,1.0);
	else if (Firing > 0)
        FragData[MAP_FIRINGS] = vec4(FireColor,1.0);
    else
        FragData[MAP_FIRINGS] = vec4(IdleColor,1.0);

    PickData  = ID;
}
