#version 420

uniform mat4 Projection, View;
uniform vec3 Object_Bounds;

flat in uint ID[1];
in vec3 Position[1];

flat layout(stream = 0) out uint Selected_ID;
flat layout(stream = 1) out uint Deselected_ID;

layout (points) in;
layout (points,max_vertices =1) out;

bool inFrustum(vec4 point)
{
    return (- point.w) <= point.x && point.x <= point.w && (- point.w )<= point.y && point.y <= point.w && (- point.w) <= point.z && point.z <= point.w;
}

void main(void)
{

    Selected_ID = ID[0];
    bool neuronInFrustum = true;
    for (float x = -Object_Bounds.x/2.0f; x <= Object_Bounds.x/2.0f; x+= Object_Bounds.x)
        for (float y = -Object_Bounds.y/2.0f; y <= Object_Bounds.y/2.0f; y+= Object_Bounds.y)
            for (float z = -Object_Bounds.z/2.0f; z <= Object_Bounds.z/2.0f; z+= Object_Bounds.z)
            {
                vec4 frustumPos = Projection * View *vec4(Position[0] + vec3(x,y,z),1.0f);
                if (!inFrustum(frustumPos))
                    neuronInFrustum =false;
            }

    if (neuronInFrustum)
    {
        Selected_ID = ID[0];
        EmitVertex();
        EndPrimitive();
    }
    else
    {
        Deselected_ID = ID[0];
        EmitVertex();
        EndPrimitive();
    }
}
