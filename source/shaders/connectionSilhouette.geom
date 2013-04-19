#version 330

uniform mat4 WVP,Scale;
uniform vec3 CameraDirection;

layout (lines) in;
layout (line_strip,max_vertices =6) out;

void emitLine(vec3 start, vec3 end)
{
    gl_Position = WVP *vec4(start,1.0f);
    EmitVertex();

    gl_Position = WVP *vec4(end,1.0f);
    EmitVertex();
    EndPrimitive();
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

    vec3 leftFaceNormal =  (lineUp- lineRight)/2;
    vec3 rightFaceNormal =  (lineUp+ lineRight)/2;
    vec3 downFaceNormal =  -lineUp;

    bool leftFacingOut = dot(leftFaceNormal, CameraDirection) < 0;
    bool rightFacingOut = dot(rightFaceNormal, CameraDirection) < 0;
    bool downFacingOut = dot(downFaceNormal, CameraDirection) < 0;

    if (leftFacingOut != rightFacingOut)
        emitLine(inUpPos,outUpPos);

    if (leftFacingOut != downFacingOut)
        emitLine(inDownLeftPos,outDownLeftPos);

    if (downFacingOut != rightFacingOut)
        emitLine(inDownRightPos,outDownRightPos);

}


