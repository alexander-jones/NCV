#version 330

uniform mat4 WVP , Scale;
uniform float FarPlane,NearPlane, DepthBias,FarBias;

flat in uint Vert_ID[2];
flat in uint Vert_Value[2];

flat out uint ID;
flat out uint Value;
out float logz;

layout (lines) in;
layout (triangle_strip,max_vertices =8) out;

void emitVertex(vec3 pos )
{
    gl_Position = WVP * vec4(pos,1.0f);
    logz = log(gl_Position.w*DepthBias + 1)*FarBias;
    gl_Position.z = (2*logz - 1)*gl_Position.w;
    EmitVertex();
}

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
    Value = Vert_Value[0];

    emitVertex(inUpPos);

    emitVertex(outUpPos);

    emitVertex(inDownLeftPos);

    emitVertex(outDownLeftPos);


    //////////////////////////// Down Face ////////////////////////////

    emitVertex(inDownRightPos);

    emitVertex(outDownRightPos);

    //////////////////////////// Right-Up Face ////////////////////////////

    emitVertex(inUpPos);

    emitVertex(outUpPos);

    EndPrimitive();
}

