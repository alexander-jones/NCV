#version 400

uniform mat4 Projection;
uniform float ConnectionWidth;

flat in uint Vert_ID[2];
flat in uint Vert_BitFlag[2];
in vec3 Vert_WorldPos[2];
in float Vert_Depth[2];

flat out uint ID;
flat out uint BitFlag;
out vec3 WorldPos, Normal;
out float Depth;

layout (lines) in;
layout (triangle_strip,max_vertices =16) out;

void main( void )
{

    // establish view space offets for right, up, left (- right), down (- up) faces.
    vec3 inPositionView = gl_in[0].gl_Position.xyz;
    vec3 outPositionView = gl_in[1].gl_Position.xyz;
    vec3 normLineDir = normalize(outPositionView-inPositionView);
    vec3 rightFaceOffset=cross(normLineDir,vec3(0.0,1.0,0.0));
    if(abs(normLineDir.y)>0.999)
        rightFaceOffset=cross(normLineDir,vec3(1.0,0.0,0.0));
    rightFaceOffset=normalize(rightFaceOffset) * ConnectionWidth;
    vec3 upFaceOffset=cross(normLineDir,rightFaceOffset) * ConnectionWidth;

    // establish world space normals for right, up, left (- right), down (- up) faces.
    vec3 worldPosLineDir = normalize(Vert_WorldPos[1] -Vert_WorldPos[0]);
    vec3 rightFaceNormal = cross(worldPosLineDir,vec3(0,1,0));
    if(abs(rightFaceNormal.y)>0.999)
        rightFaceNormal = cross(worldPosLineDir,vec3(1,0,0));
    vec3 upFaceNormal = cross(worldPosLineDir,rightFaceNormal);

    // establish line vertices in projection space
    vec4 inDownLeftPos = Projection * vec4( inPositionView -rightFaceOffset-upFaceOffset , 1.0);
    vec4 inUpLeftPos = Projection * vec4( inPositionView -rightFaceOffset+upFaceOffset , 1.0);
    vec4 inUpRightPos = Projection * vec4( inPositionView +rightFaceOffset+upFaceOffset , 1.0);
    vec4 inDownRightPos = Projection * vec4( inPositionView +rightFaceOffset-upFaceOffset , 1.0);

    vec4 outDownLeftPos = Projection * vec4( outPositionView -rightFaceOffset-upFaceOffset , 1.0);
    vec4 outUpLeftPos = Projection * vec4( outPositionView -rightFaceOffset+upFaceOffset , 1.0);
    vec4 outUpRightPos = Projection * vec4( outPositionView +rightFaceOffset+upFaceOffset , 1.0);
    vec4 outDownRightPos = Projection * vec4( outPositionView +rightFaceOffset-upFaceOffset , 1.0);

    ID = Vert_ID[0];

    //////////////////////////// Left Face ////////////////////////////

    // setup out values for neuron-in located points
    BitFlag = Vert_BitFlag[0];
    WorldPos = Vert_WorldPos[0];
    Depth = Vert_Depth[0];

    Normal = - rightFaceNormal;
    gl_Position = inUpLeftPos;
    EmitVertex();

    gl_Position = inDownLeftPos;
    EmitVertex();


    // setup out values for neuron-out located points
    BitFlag = Vert_BitFlag[1];
    WorldPos = Vert_WorldPos[1];
    Depth = Vert_Depth[1];

    gl_Position = outUpLeftPos;
    EmitVertex();

    gl_Position = outDownLeftPos;
    EmitVertex();

    //////////////////////////// Right Face ////////////////////////////

    Normal = rightFaceNormal;
    gl_Position = outUpRightPos;
    EmitVertex();

    gl_Position = outDownRightPos;
    EmitVertex();

    // setup out values for neuron-in located points
    BitFlag = Vert_BitFlag[0];
    WorldPos = Vert_WorldPos[0];
    Depth = Vert_Depth[0];

    gl_Position = inUpRightPos;
    EmitVertex();

    gl_Position = inDownRightPos;
    EmitVertex();

    //////////////////////////// Up Face ////////////////////////////

    // setup out values for neuron-out located points
    BitFlag = Vert_BitFlag[1];
    WorldPos = Vert_WorldPos[1];
    Depth = Vert_Depth[1];

    Normal =  upFaceNormal;
    gl_Position = outUpLeftPos;
    EmitVertex();

    gl_Position = outUpRightPos;
    EmitVertex();


    // setup out values for neuron-in located points
    BitFlag = Vert_BitFlag[0];
    WorldPos = Vert_WorldPos[0];
    Depth = Vert_Depth[0];


    gl_Position = inUpLeftPos;
    EmitVertex();

    gl_Position = inUpRightPos;
    EmitVertex();

    //////////////////////////// Down Face ////////////////////////////

    Normal = - upFaceNormal;
    gl_Position = inDownLeftPos;
    EmitVertex();

    gl_Position = inDownRightPos;
    EmitVertex();

    // setup out values for neuron-out located points
    BitFlag = Vert_BitFlag[1];
    WorldPos = Vert_WorldPos[1];
    Depth = Vert_Depth[1];

    gl_Position = outDownLeftPos;
    EmitVertex();

    gl_Position = outDownRightPos;
    EmitVertex();

    EndPrimitive();
}

