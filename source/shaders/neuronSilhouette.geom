uniform mat4 WVP;
uniform vec3 CameraPosition;

layout (triangles_adjacency) in;
layout (line_strip,max_vertices =6) out;

void EmitLine(vec3 start, vec3 end)
{
    gl_Position = WVP * gl_in[StartIndex].gl_Position;
    EmitVertex();

    gl_Position = WVP * gl_in[EndIndex].gl_Position;
    EmitVertex();

    EndPrimitive();
}

void main(void)
{
    vec3 e1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 e2 = gl_in[4].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 e3 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 e4 = gl_in[3].gl_Position.xyz - gl_in[2].gl_Position.xyz;
    vec3 e5 = gl_in[4].gl_Position.xyz - gl_in[2].gl_Position.xyz;
    vec3 e6 = gl_in[5].gl_Position.xyz - gl_in[0].gl_Position.xyz;

    vec3 normal = cross(e1,e2);
    vec3 silhoutteDir = CameraPosition - gl_in[0].gl_Position.xyz;

    if (dot(normal, silhoutteDir) > 0.00001) {

        normal = cross(e3,e1);

        if (dot(normal, silhoutteDir) <= 0) {
            EmitLine(0, 2);
        }

        normal = cross(e4,e5);
        silhoutteDir = CameraPosition - gl_in[2].gl_Position.xyz;

        if (dot(normal, silhoutteDir) <=0) {
            EmitLine(2, 4);
        }

        normal = cross(e2,e6);
        silhoutteDir = CameraPosition - gl_in[4].gl_Position.xyz;

        if (dot(normal, silhoutteDir) <= 0) {
            EmitLine(4, 0);
        }
    }
}

