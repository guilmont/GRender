#version 450 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in vec3 bCOM;
layout(location = 4) in vec3 bColor;

uniform mat4 u_transform;

out vec3 fPos;
out vec3 fNormal;
out vec3 fColor;

void main() {
    vec3 loc = vPosition + bCOM;
    gl_Position = u_transform * vec4(loc, 1.0);
    
    fPos = loc;
    fNormal = vNormal;
    fColor = bColor;
}

