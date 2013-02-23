#version 330

uniform mat4 WVP,Scale;

flat in uint Vert_ID[2];
in float Vert_Value[2];

flat out uint ID;
out float Value;
out vec3  Normal;

layout (lines) in;
layout (triangle_strip,max_vertices =10) out;

void main( void )
{
    // establish view space offets for right, up, left (- right), down (- up) faces.
    vec3 inPosition = gl_in[0].gl_Position.xyz;
    vec3 outPosition = gl_in[1].gl_Position.xyz;

    // establish line direction
    vec3 lineDir = normalize(outPosition -inPosition);

    // establish right perpindicular by crossing line direction with y axis.
    vec3 lineRight = normalize(cross(lineDir,vec3(0,1,0)));

    // if line direction is pointing straight up, instead perform cross product with x axis
    // to establish right perpindicular.
    if(abs(lineDir.y) > 0.999)
        lineRight = normalize(cross(lineDir,vec3(1,0,0)));

    // establish left perpindicular by crossing line direction right perpindicular.
    vec3 lineUp = normalize(cross(lineDir,lineRight));

    // establish offsets for the right and up faces of the volumetric line
    // left face == - right face and down face == - up face
    vec3 lineRightOffset = (Scale * vec4(lineRight,1.0f)).xyz;
    vec3 lineUpOffset = (Scale * vec4(lineUp,1.0f)).xyz;

    // establish line vertices
    vec3 inDownLeftPos =  inPosition -lineRightOffset-lineUpOffset;
    vec3 inUpPos = inPosition +lineUpOffset;
    vec3 inDownRightPos = inPosition +lineRightOffset-lineUpOffset ;

    vec3 outDownLeftPos =  outPosition -lineRightOffset-lineUpOffset ;
    vec3 outUpPos =  outPosition +lineUpOffset;
    vec3 outDownRightPos =  outPosition +lineRightOffset-lineUpOffset ;

    ID = Vert_ID[0];

    //////////////////////////// Left-Up Face ////////////////////////////

    // setup out values for neuron-in located points
    Value = Vert_Value[0];

    Normal = (lineUp- lineRight)/2;

    gl_Position = WVP *vec4(inUpPos,1.0f);
    EmitVertex();

    gl_Position = WVP *vec4(inDownLeftPos,1.0f);
    EmitVertex();

    // setup out values for neuron-out located points
    Value = Vert_Value[0];

    gl_Position = WVP *vec4(outUpPos,1.0f);
    EmitVertex();

    gl_Position = WVP *vec4(outDownLeftPos,1.0f);
    EmitVertex();


    //////////////////////////// Down Face ////////////////////////////

    Normal = - lineUp;

    gl_Position = WVP *vec4(outDownRightPos,1.0f);
    EmitVertex();

    // setup out values for neuron-out located points
    Value = Vert_Value[0];

    gl_Position = WVP *vec4(inDownLeftPos,1.0f);
    EmitVertex();

    gl_Position = WVP *vec4(inDownRightPos,1.0f);
    EmitVertex();

    //////////////////////////// Right-Up Face ////////////////////////////

    Normal = (lineUp + lineRight)/2;

    gl_Position = WVP *vec4(inUpPos,1.0f);
    EmitVertex();

    // setup out values for neuron-in located points
    Value = Vert_Value[0];

    gl_Position = WVP *vec4(outDownRightPos,1.0f);
    EmitVertex();

    gl_Position = WVP *vec4(outUpPos,1.0f);
    EmitVertex();

    EndPrimitive();
}


