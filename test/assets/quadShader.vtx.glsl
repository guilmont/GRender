#version 430 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in float vTexID;

out vec4 fColor;
out vec2 fCoord;
out float texID;

uniform mat4 u_viewProjection;

void main()
{
    fColor = vColor;
    fCoord = vTexCoord;
    texID = vTexID;

    gl_Position = u_viewProjection * vec4(vPos, 1.0);
}

