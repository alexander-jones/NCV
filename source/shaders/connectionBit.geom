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

    vec3 inPosition = gl_in[0].gl_Position.xyz;
    vec3 outPosition = gl_in[1].gl_Position.xyz;

    vec3 lineDir = outPosition-inPosition;
    vec3 normLineDir = normalize(lineDir);

    vec3 normLineRight=cross(normLineDir,vec3(0.0,1.0,0.0));
    if(abs(normLineDir.y)>0.999)
            normLineRight=cross(normLineDir,vec3(1.0,0.0,0.0));
    normLineRight=normalize(normLineRight) * ConnectionWidth;
    vec3 normLineUp=cross(normLineDir,normLineRight) * ConnectionWidth;


    // establish line vertices in projection space
    vec4 inDownLeftPos = Projection * vec4( inPosition -normLineRight-normLineUp , 1.0);
    vec4 inUpLeftPos = Projection * vec4( inPosition -normLineRight+normLineUp , 1.0);
    vec4 inUpRightPos = Projection * vec4( inPosition +normLineRight+normLineUp , 1.0);
    vec4 inDownRightPos = Projection * vec4( inPosition +normLineRight-normLineUp , 1.0);

    vec4 outDownLeftPos = Projection * vec4( outPosition -normLineRight-normLineUp , 1.0);
    vec4 outUpLeftPos = Projection * vec4( outPosition -normLineRight+normLineUp , 1.0);
    vec4 outUpRightPos = Projection * vec4( outPosition +normLineRight+normLineUp , 1.0);
    vec4 outDownRightPos = Projection * vec4( outPosition +normLineRight-normLineUp , 1.0);

    ID = Vert_ID[0];

    BitFlag = Vert_BitFlag[0];
    WorldPos = Vert_WorldPos[0];
    Depth = Vert_Depth[0];

    gl_Position = inUpLeftPos;
    EmitVertex();/////////////////////////////////////

    gl_Position = inDownLeftPos;
    EmitVertex();/////////////////////////////////////


    BitFlag = Vert_BitFlag[1];
    WorldPos = Vert_WorldPos[1];
    Depth = Vert_Depth[1];

    gl_Position = outUpLeftPos;
    EmitVertex();/////////////////////////////////////

    gl_Position = outDownLeftPos;
    EmitVertex();/////////////////////////////////////

    gl_Position = outUpRightPos;
    EmitVertex();/////////////////////////////////////

    gl_Position = outDownRightPos;
    EmitVertex();/////////////////////////////////////

    BitFlag = Vert_BitFlag[0];
    WorldPos = Vert_WorldPos[0];
    Depth = Vert_Depth[0];

    gl_Position = inUpRightPos;
    EmitVertex();/////////////////////////////////////

    gl_Position = inDownRightPos;
    EmitVertex();/////////////////////////////////////

    EndPrimitive();//////////////////////////////////////////////////////////////////////////

    BitFlag = Vert_BitFlag[1];
    WorldPos = Vert_WorldPos[1];
    Depth = Vert_Depth[1];

    gl_Position = outUpLeftPos;
    EmitVertex();/////////////////////////////////////

    gl_Position = outUpRightPos;
    EmitVertex();/////////////////////////////////////


    BitFlag = Vert_BitFlag[0];
    WorldPos = Vert_WorldPos[0];
    Depth = Vert_Depth[0];

    gl_Position = inUpLeftPos;
    EmitVertex();/////////////////////////////////////

    gl_Position = inUpRightPos;
    EmitVertex();/////////////////////////////////////

    gl_Position = inDownLeftPos;
    EmitVertex();/////////////////////////////////////

    gl_Position = inDownRightPos;
    EmitVertex();/////////////////////////////////////

    BitFlag = Vert_BitFlag[1];
    WorldPos = Vert_WorldPos[1];
    Depth = Vert_Depth[1];

    gl_Position = outDownLeftPos;
    EmitVertex();/////////////////////////////////////

    gl_Position = outDownRightPos;
    EmitVertex();/////////////////////////////////////

    EndPrimitive();//////////////////////////////////////////////////////////////////////////
}

