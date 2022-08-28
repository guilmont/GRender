#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec3 instance;

uniform mat4 u_transform;
uniform float u_radius;

out vec3 fPos;
out vec3 fNormal;
out vec3 fColor;

void main()
{
    vec3 loc = u_radius*pos + instance;

    gl_Position = u_transform * vec4(loc, 1.0);
    
    fPos = loc;
    fNormal = normal;
    fColor = color;
}

